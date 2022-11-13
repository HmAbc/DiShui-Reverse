#include <Windows.h>
#include <tchar.h>
#include "Utils.h"
#define KEY 0xD9876543

typedef long (__stdcall *pfnZwUnmapViewOfSection)(HANDLE, LPVOID);

int APIENTRY _tWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR lpCmdLine,
    _In_ int nShowCmd
)
{
    DWORD fileSize = 0;
    PIMAGE_DOS_HEADER dosHeader = NULL;
    PIMAGE_NT_HEADERS ntHeader = NULL;
    PIMAGE_FILE_HEADER peHeader = NULL;
    PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;
    PIMAGE_SECTION_HEADER sectionHeader = NULL;
    PIMAGE_SECTION_HEADER lastSectionHeader = NULL;

    //读取当前进程路径
    TCHAR currentPath[256] = { 0 };
    GetModuleFileName(NULL, currentPath, 256);
    
    //获取文件头
    dosHeader = (PIMAGE_DOS_HEADER)hInstance;
    ntHeader = (PIMAGE_NT_HEADERS)((DWORD)hInstance + dosHeader->e_lfanew);
    peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
    optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);
    sectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)optionalHeader + peHeader->SizeOfOptionalHeader);
    
    //获取最后一节数据，先判断节名，如果节名是“PYF”，提取数据进行解密
    DWORD index = peHeader->NumberOfSections;
    lastSectionHeader = sectionHeader + index - 1;
    CHAR sectionName[8] = { 0 };
    memcpy(sectionName, lastSectionHeader->Name, 8);

    if (strcmp(sectionName, "PYF"))
    {
        MessageBox(NULL, TEXT("文件未加壳或被修改"), TEXT("打开失败"), 0);
        return -1;
    }
    
    //提取最后一节的数据，并解密
    PDWORD originFile = (PDWORD)malloc(lastSectionHeader->SizeOfRawData);
    if (!originFile)
    {
        return -1;
    }
    memset(originFile, 0, lastSectionHeader->SizeOfRawData);
    //把最后一节数据解密到新申请的内存空间中
    for (DWORD i = 0; i < lastSectionHeader->SizeOfRawData / 4; i++)
    {
        originFile[i] = ((PDWORD)((DWORD)hInstance + lastSectionHeader->VirtualAddress))[i] ^ KEY;
    }

    //获取内核函数，用来卸载程序
    pfnZwUnmapViewOfSection ZwUnmapViewOfSection;
    HMODULE ntdllModule = GetModuleHandle(TEXT("ntdll.dll"));
    if (!ntdllModule)
    {
        return -1;
    }
    ZwUnmapViewOfSection = (pfnZwUnmapViewOfSection)GetProcAddress(ntdllModule, "ZwUnmapViewOfSection");
    if (!ZwUnmapViewOfSection)
    {
        return -1;
    }

    //以挂起的形式创建进程
    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(si);
    CreateProcess(NULL, currentPath, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);

    //获取外壳程序的Context
    CONTEXT context;
    context.ContextFlags = CONTEXT_FULL;
    GetThreadContext(pi.hThread, &context);
    //获取 imagebase 和 oep
    DWORD entryPoint = context.Eax;
    CHAR* baseAddress = (CHAR*)context.Ebx + 8;
    DWORD imageBase = 0;
    DWORD readSize = 0;
    ReadProcessMemory(pi.hProcess, baseAddress, &imageBase, 4, &readSize);

    //卸载文件外壳镜像
    ZwUnmapViewOfSection(pi.hThread, (LPVOID)imageBase);

    //拉伸文件到内存
    LPVOID imageBuffer = NULL;
    CopyFileBufferToImageBuffer((LPVOID)originFile, &imageBuffer);
    if (!imageBase)
    {
        free(originFile);
        TerminateThread(pi.hThread, 2);
        WaitForSingleObject(pi.hThread, INFINITE);
        return -1;
    }
    free(originFile);
    
    //获取 src 程序的imagebase，sizeofimage
    DWORD srcImageBase = 0, srcSizeOfImage = 0, srcOEP = 0;
    GetProcessBase(imageBuffer, srcImageBase, srcSizeOfImage, srcOEP);

    //在挂起的程序中申请一段内存空间
    LPVOID status = NULL;
    status = VirtualAllocEx(pi.hProcess, (LPVOID)srcImageBase, srcSizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    //如果申请失败
    if (!status)
    {
        //判断有没有重定位表，若没有
        if (!IsRelocation(imageBuffer))
        {
            free(imageBuffer);
            TerminateThread(pi.hThread, 2);
            WaitForSingleObject(pi.hThread, INFINITE);
            return -1;
        }
        //若有，重新申请空间，再修复重定位表
        status = VirtualAllocEx(pi.hProcess, NULL, srcSizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (!status)
        {
            TerminateThread(pi.hThread, 2);
            WaitForSingleObject(pi.hThread, INFINITE);
            return -1;
        }
        //申请成功，修复重定位表
        RepaireRelocation(imageBuffer, (DWORD)status);
    }

    //把 src 程序写入外壳程序中申请的内存中
    SIZE_T srcResult = 0;
    WriteProcessMemory(pi.hProcess, status, imageBuffer, srcSizeOfImage, &srcResult);

    //修正context结构
    WriteProcessMemory(pi.hProcess, baseAddress, &status, 4, NULL);     //修改新的imagebase
    context.Eax = srcOEP + srcImageBase;        //修改新的entrypoint
    
    //设置新的context
    SetThreadContext(pi.hThread, &context);
    //恢复线程
    ResumeThread(pi.hThread);

    free(imageBuffer);
    return 0;
}
