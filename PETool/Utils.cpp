#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include "Utils.h"

void __cdecl OutputDebugStringF(const char* format, ...)
{
	va_list vlArgs;
	char* strBuffer = (char*)GlobalAlloc(GPTR, 4096);

	va_start(vlArgs, format);
	_vsnprintf(strBuffer, 4096 - 1, format, vlArgs);
	va_end(vlArgs);
	strcat(strBuffer, "\n");
	OutputDebugStringA(strBuffer);
	GlobalFree(strBuffer);
	return;
}

/// @brief 设置进程访问权限，针对系统进程
/// @param lpName 需要的权限的名称
/// @param opt 选择是否设置权限
/// @return 成功返回TRUE
BOOL SetProcessPrivilege(IN LPCWSTR lpName, IN BOOL opt)
{
	TOKEN_PRIVILEGES tp;
	HANDLE tokenHandle;

	//打开本程序权限token
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &tokenHandle))
	{
		//函数查看系统权限的特权值，返回信息到一个LUID结构体里
		LookupPrivilegeValue(NULL, lpName, &tp.Privileges[0].Luid);
		tp.PrivilegeCount = 1;
		tp.Privileges->Attributes = (opt != 0 ? SE_PRIVILEGE_ENABLED : 0);	//打开权限

		//开启token的权限，如果有的话，没有权限的话就无法打开
		AdjustTokenPrivileges(tokenHandle, FALSE, &tp, sizeof(tp), NULL, NULL);
		CloseHandle(tokenHandle);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/// @brief 打开PE文件，读取PE信息，显示在IDD_DIALOG_PE PE信息查看 对话框
/// @param filePath PE文件路径
/// @param fileBuffer 指针，打开成功后指向PE文件在内存的地址
/// @return 成功返回TRUE
DWORD ReadPEFile(IN LPCTSTR filePath, OUT LPVOID* fileBuffer)
{
	FILE* file = NULL;
	DWORD fileSize = 0;
	LPVOID fileBufferTemp = NULL;
	PIMAGE_DOS_HEADER dosHeader;


	_tfopen_s(&file, filePath, TEXT("rb"));
	if (!file)
	{
		return 0;
	}

	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	fileBufferTemp = malloc(fileSize);
	if (!fileBufferTemp)
	{
		fclose(file);
		return 0;
	}

	DWORD n = fread(fileBufferTemp, sizeof(BYTE), fileSize, file);
	if (!n)
	{
		fclose(file);
		free(fileBufferTemp);
		return 0;
	}

	if (*(PWORD)fileBufferTemp != IMAGE_DOS_SIGNATURE)
	{
		fclose(file);
		free(fileBufferTemp);
		return 0;
	}

	dosHeader = (PIMAGE_DOS_HEADER)fileBufferTemp;
	if (*(PDWORD)((DWORD)fileBufferTemp + dosHeader->e_lfanew) != IMAGE_NT_SIGNATURE)
	{
		fclose(file);
		free(fileBufferTemp);
		return 0;
	}

	*fileBuffer = fileBufferTemp;
	fileBufferTemp = NULL;
	fclose(file);
	return fileSize;
}

/// @brief 将PE文件执行时的RVA转换为FOA
/// @param fileBuffer PE文件读取后在内存中的地址
/// @param rva 文件执行后的RVA
/// @return 返回rva对应的foa，超出文件大小的rva返回0
DWORD RVA2FOA(IN LPVOID fileBuffer, IN DWORD rva)
{
	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS ntHeader = NULL;
	PIMAGE_FILE_HEADER	peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;
	PIMAGE_SECTION_HEADER sectionHeader = NULL;
	DWORD maxSize = 0;

	dosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	ntHeader = (PIMAGE_NT_HEADERS)((DWORD)fileBuffer + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);
	sectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)optionalHeader + peHeader->SizeOfOptionalHeader);

	if (rva < optionalHeader->SizeOfHeaders)
	{
		return rva;
	}

	for (DWORD i = 0; i < peHeader->NumberOfSections; i++)
	{
		maxSize = sectionHeader[i].Misc.VirtualSize > sectionHeader[i].SizeOfRawData ? sectionHeader[i].Misc.VirtualSize : sectionHeader[i].SizeOfRawData;
		if (sectionHeader[i].VirtualAddress + maxSize > rva)
		{
			return rva - sectionHeader[i].VirtualAddress + sectionHeader[i].PointerToRawData;
		}
	}

	return 0;
}

/// @brief 将ASCII码字符串转换成UNICODE
/// @param source ASCII码字符串地址
/// @param result 接收转换后字符串的缓冲区地址
VOID ANSIToUnicode(IN PCSTR source, OUT LPWSTR result)
{
	int length = 0;
	length = MultiByteToWideChar(CP_ACP, 0, source, -1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, source, -1, result, length);
}

/// @brief 将UNICODE字符串转换成ASCII码字符串
/// @param source UNICODE字符串地址
/// @param result 接收转换后字符串的缓冲区地址
VOID UnicodeToANSI(IN LPCWSTR source, OUT LPSTR result)
{
	int length = 0;
	length = WideCharToMultiByte(CP_ACP, 0, source, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, source, -1, result, length, NULL, NULL);
}

/// @brief 将PE头文件中的NTHeader处向前移动到DOSHeader->e_lfanew后
/// @param fileBuffer 文件缓冲区地址
/// @return 成功返回TRUE，失败返回FALSE
BOOL MoveHeader(IN LPVOID fileBuffer)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS32 pNtHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;

	LPVOID pNewNtHeader = NULL;
	DWORD size = 0;

	pDosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	//如果dosheader和ntheader之间不够一个节大小，不移动
	if (pDosHeader->e_lfanew - 64 < IMAGE_SIZEOF_SECTION_HEADER)
	{
		return 0;
	}

	pNtHeader = (PIMAGE_NT_HEADERS32)((DWORD)fileBuffer + pDosHeader->e_lfanew);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	//目的起始地址
	pNewNtHeader = (LPVOID)((DWORD)(fileBuffer)+64);
	//要移动的大小
	size = 4 + IMAGE_SIZEOF_FILE_HEADER + pPEHeader->SizeOfOptionalHeader + (pPEHeader->NumberOfSections) * 40;
	//开始移动
	memcpy(pNewNtHeader, (LPVOID)pNtHeader, size);
	//在最后一个section后面添加一个全0的section
	memset((LPVOID)((DWORD)(fileBuffer)+64 + size), 0, 40);
	//设置elfanew的值
	((PIMAGE_DOS_HEADER)(fileBuffer))->e_lfanew = 64;

	return TRUE;
}

/// @brief 在PE文件最后添加一个节，可以加在拉伸或未拉伸的文件后，默认添加属性为0xE0000060
/// @param pImageBuffer 文件地址
/// @param pNewImageBuffer 添加完section的文件地址
/// @param fileSize 需要添加节的文件的大小
/// @param addSize 要增加的数据大小
/// @param name 要增加节的名字，字节数组
/// @return 成功返回总的文件大小，pNewImageBuffer保存添加section后的文件地址，失败返回0
DWORD AddNewSection(IN LPVOID pImageBuffer, OUT LPVOID* pNewImageBuffer, IN DWORD fileSize, IN DWORD addSize, IN PCHAR name)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS32 pNtHeader = NULL;
	PIMAGE_FILE_HEADER pPEHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 pOptionHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	DWORD index = 0;
	DWORD newFileSize = 0;
	DWORD virtualSize = 0;
	PIMAGE_SECTION_HEADER newSectionHeader = NULL;
	LPVOID pNewImageBufferTemp = NULL;

	virtualSize = ((addSize - 1) / 0x1000 + 1) * 0x1000;
	newFileSize = fileSize + virtualSize;
	pNewImageBufferTemp = malloc(newFileSize);
	if (!pNewImageBufferTemp)
	{
		free(pImageBuffer);
		return 0;
	}

	memset(pNewImageBufferTemp, 0, newFileSize);
	memcpy(pNewImageBufferTemp, pImageBuffer, fileSize);

	pDosHeader = (PIMAGE_DOS_HEADER)pNewImageBufferTemp;
	pNtHeader = (PIMAGE_NT_HEADERS32)((DWORD)pNewImageBufferTemp + pDosHeader->e_lfanew);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	index = pPEHeader->NumberOfSections;
	newSectionHeader = pSectionHeader + index;
	memcpy(newSectionHeader->Name, name, 8);
	newSectionHeader->Misc.VirtualSize = virtualSize;
	newSectionHeader->VirtualAddress = pOptionHeader->SizeOfImage;
	newSectionHeader->SizeOfRawData = addSize;
	newSectionHeader->PointerToRawData = pSectionHeader[index - 1].PointerToRawData + pSectionHeader[index - 1].SizeOfRawData;
	newSectionHeader->PointerToRelocations = 0;
	newSectionHeader->PointerToLinenumbers = 0;
	newSectionHeader->NumberOfRelocations = 0;
	newSectionHeader->NumberOfLinenumbers = 0;
	newSectionHeader->Characteristics = 0xE0000060;
	//在节表后添加一个全零节表，表示结束
	memset(newSectionHeader + 1, 0, sizeof(IMAGE_SECTION_HEADER));

	pPEHeader->NumberOfSections += 1;
	pOptionHeader->SizeOfImage += virtualSize;

	*pNewImageBuffer = pNewImageBufferTemp;
	pNewImageBufferTemp = NULL;
	return newFileSize;
}

/// @brief 从内存将文件写回磁盘
/// @param pMemBuffer 需要保存的文件在内存中的地址
/// @param size 需要保存的文件的大小
/// @param lpszFile 保存文件路径及文件名
/// @return 成功返回TRUE，失败返回FALSE
BOOL MemoryToFile(IN LPVOID pMemBuffer, IN size_t size, OUT LPCTSTR lpszFile)
{
	FILE* pFile = NULL;
	errno_t err = 0;
	err = _tfopen_s(&pFile, lpszFile, TEXT("wb+"));
	if (!pFile)
	{
		printf("(MemoryToFile)文件创建失败,错误代码：%d\n", err);
		free(pMemBuffer);
		return FALSE;
	}

	fwrite(pMemBuffer, sizeof(BYTE), size, pFile);
	fclose(pFile);
	return TRUE;
}