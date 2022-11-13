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

    //��ȡ��ǰ����·��
    TCHAR currentPath[256] = { 0 };
    GetModuleFileName(NULL, currentPath, 256);
    
    //��ȡ�ļ�ͷ
    dosHeader = (PIMAGE_DOS_HEADER)hInstance;
    ntHeader = (PIMAGE_NT_HEADERS)((DWORD)hInstance + dosHeader->e_lfanew);
    peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
    optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);
    sectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)optionalHeader + peHeader->SizeOfOptionalHeader);
    
    //��ȡ���һ�����ݣ����жϽ�������������ǡ�PYF������ȡ���ݽ��н���
    DWORD index = peHeader->NumberOfSections;
    lastSectionHeader = sectionHeader + index - 1;
    CHAR sectionName[8] = { 0 };
    memcpy(sectionName, lastSectionHeader->Name, 8);

    if (strcmp(sectionName, "PYF"))
    {
        MessageBox(NULL, TEXT("�ļ�δ�ӿǻ��޸�"), TEXT("��ʧ��"), 0);
        return -1;
    }
    
    //��ȡ���һ�ڵ����ݣ�������
    PDWORD originFile = (PDWORD)malloc(lastSectionHeader->SizeOfRawData);
    if (!originFile)
    {
        return -1;
    }
    memset(originFile, 0, lastSectionHeader->SizeOfRawData);
    //�����һ�����ݽ��ܵ���������ڴ�ռ���
    for (DWORD i = 0; i < lastSectionHeader->SizeOfRawData / 4; i++)
    {
        originFile[i] = ((PDWORD)((DWORD)hInstance + lastSectionHeader->VirtualAddress))[i] ^ KEY;
    }

    //��ȡ�ں˺���������ж�س���
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

    //�Թ������ʽ��������
    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(si);
    CreateProcess(NULL, currentPath, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);

    //��ȡ��ǳ����Context
    CONTEXT context;
    context.ContextFlags = CONTEXT_FULL;
    GetThreadContext(pi.hThread, &context);
    //��ȡ imagebase �� oep
    DWORD entryPoint = context.Eax;
    CHAR* baseAddress = (CHAR*)context.Ebx + 8;
    DWORD imageBase = 0;
    DWORD readSize = 0;
    ReadProcessMemory(pi.hProcess, baseAddress, &imageBase, 4, &readSize);

    //ж���ļ���Ǿ���
    ZwUnmapViewOfSection(pi.hThread, (LPVOID)imageBase);

    //�����ļ����ڴ�
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
    
    //��ȡ src �����imagebase��sizeofimage
    DWORD srcImageBase = 0, srcSizeOfImage = 0, srcOEP = 0;
    GetProcessBase(imageBuffer, srcImageBase, srcSizeOfImage, srcOEP);

    //�ڹ���ĳ���������һ���ڴ�ռ�
    LPVOID status = NULL;
    status = VirtualAllocEx(pi.hProcess, (LPVOID)srcImageBase, srcSizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    //�������ʧ��
    if (!status)
    {
        //�ж���û���ض�λ����û��
        if (!IsRelocation(imageBuffer))
        {
            free(imageBuffer);
            TerminateThread(pi.hThread, 2);
            WaitForSingleObject(pi.hThread, INFINITE);
            return -1;
        }
        //���У���������ռ䣬���޸��ض�λ��
        status = VirtualAllocEx(pi.hProcess, NULL, srcSizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        if (!status)
        {
            TerminateThread(pi.hThread, 2);
            WaitForSingleObject(pi.hThread, INFINITE);
            return -1;
        }
        //����ɹ����޸��ض�λ��
        RepaireRelocation(imageBuffer, (DWORD)status);
    }

    //�� src ����д����ǳ�����������ڴ���
    SIZE_T srcResult = 0;
    WriteProcessMemory(pi.hProcess, status, imageBuffer, srcSizeOfImage, &srcResult);

    //����context�ṹ
    WriteProcessMemory(pi.hProcess, baseAddress, &status, 4, NULL);     //�޸��µ�imagebase
    context.Eax = srcOEP + srcImageBase;        //�޸��µ�entrypoint
    
    //�����µ�context
    SetThreadContext(pi.hThread, &context);
    //�ָ��߳�
    ResumeThread(pi.hThread);

    free(imageBuffer);
    return 0;
}
