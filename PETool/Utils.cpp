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

/// @brief ���ý��̷���Ȩ�ޣ����ϵͳ����
/// @param lpName ��Ҫ��Ȩ�޵�����
/// @param opt ѡ���Ƿ�����Ȩ��
/// @return �ɹ�����TRUE
BOOL SetProcessPrivilege(IN LPCWSTR lpName, IN BOOL opt)
{
	TOKEN_PRIVILEGES tp;
	HANDLE tokenHandle;

	//�򿪱�����Ȩ��token
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &tokenHandle))
	{
		//�����鿴ϵͳȨ�޵���Ȩֵ��������Ϣ��һ��LUID�ṹ����
		LookupPrivilegeValue(NULL, lpName, &tp.Privileges[0].Luid);
		tp.PrivilegeCount = 1;
		tp.Privileges->Attributes = (opt != 0 ? SE_PRIVILEGE_ENABLED : 0);	//��Ȩ��

		//����token��Ȩ�ޣ�����еĻ���û��Ȩ�޵Ļ����޷���
		AdjustTokenPrivileges(tokenHandle, FALSE, &tp, sizeof(tp), NULL, NULL);
		CloseHandle(tokenHandle);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/// @brief ��PE�ļ�����ȡPE��Ϣ����ʾ��IDD_DIALOG_PE PE��Ϣ�鿴 �Ի���
/// @param filePath PE�ļ�·��
/// @param fileBuffer ָ�룬�򿪳ɹ���ָ��PE�ļ����ڴ�ĵ�ַ
/// @return �ɹ�����TRUE
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

/// @brief ��PE�ļ�ִ��ʱ��RVAת��ΪFOA
/// @param fileBuffer PE�ļ���ȡ�����ڴ��еĵ�ַ
/// @param rva �ļ�ִ�к��RVA
/// @return ����rva��Ӧ��foa�������ļ���С��rva����0
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

/// @brief ��ASCII���ַ���ת����UNICODE
/// @param source ASCII���ַ�����ַ
/// @param result ����ת�����ַ����Ļ�������ַ
VOID ANSIToUnicode(IN PCSTR source, OUT LPWSTR result)
{
	int length = 0;
	length = MultiByteToWideChar(CP_ACP, 0, source, -1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, source, -1, result, length);
}

/// @brief ��UNICODE�ַ���ת����ASCII���ַ���
/// @param source UNICODE�ַ�����ַ
/// @param result ����ת�����ַ����Ļ�������ַ
VOID UnicodeToANSI(IN LPCWSTR source, OUT LPSTR result)
{
	int length = 0;
	length = WideCharToMultiByte(CP_ACP, 0, source, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, source, -1, result, length, NULL, NULL);
}

/// @brief ��PEͷ�ļ��е�NTHeader����ǰ�ƶ���DOSHeader->e_lfanew��
/// @param fileBuffer �ļ���������ַ
/// @return �ɹ�����TRUE��ʧ�ܷ���FALSE
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
	//���dosheader��ntheader֮�䲻��һ���ڴ�С�����ƶ�
	if (pDosHeader->e_lfanew - 64 < IMAGE_SIZEOF_SECTION_HEADER)
	{
		return 0;
	}

	pNtHeader = (PIMAGE_NT_HEADERS32)((DWORD)fileBuffer + pDosHeader->e_lfanew);
	pPEHeader = (PIMAGE_FILE_HEADER)((DWORD)pNtHeader + 4);
	pOptionHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)pPEHeader + IMAGE_SIZEOF_FILE_HEADER);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)pOptionHeader + pPEHeader->SizeOfOptionalHeader);

	//Ŀ����ʼ��ַ
	pNewNtHeader = (LPVOID)((DWORD)(fileBuffer)+64);
	//Ҫ�ƶ��Ĵ�С
	size = 4 + IMAGE_SIZEOF_FILE_HEADER + pPEHeader->SizeOfOptionalHeader + (pPEHeader->NumberOfSections) * 40;
	//��ʼ�ƶ�
	memcpy(pNewNtHeader, (LPVOID)pNtHeader, size);
	//�����һ��section�������һ��ȫ0��section
	memset((LPVOID)((DWORD)(fileBuffer)+64 + size), 0, 40);
	//����elfanew��ֵ
	((PIMAGE_DOS_HEADER)(fileBuffer))->e_lfanew = 64;

	return TRUE;
}

/// @brief ��PE�ļ�������һ���ڣ����Լ��������δ������ļ���Ĭ���������Ϊ0xE0000060
/// @param pImageBuffer �ļ���ַ
/// @param pNewImageBuffer �����section���ļ���ַ
/// @param fileSize ��Ҫ��ӽڵ��ļ��Ĵ�С
/// @param addSize Ҫ���ӵ����ݴ�С
/// @param name Ҫ���ӽڵ����֣��ֽ�����
/// @return �ɹ������ܵ��ļ���С��pNewImageBuffer�������section����ļ���ַ��ʧ�ܷ���0
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
	//�ڽڱ�����һ��ȫ��ڱ���ʾ����
	memset(newSectionHeader + 1, 0, sizeof(IMAGE_SECTION_HEADER));

	pPEHeader->NumberOfSections += 1;
	pOptionHeader->SizeOfImage += virtualSize;

	*pNewImageBuffer = pNewImageBufferTemp;
	pNewImageBufferTemp = NULL;
	return newFileSize;
}

/// @brief ���ڴ潫�ļ�д�ش���
/// @param pMemBuffer ��Ҫ������ļ����ڴ��еĵ�ַ
/// @param size ��Ҫ������ļ��Ĵ�С
/// @param lpszFile �����ļ�·�����ļ���
/// @return �ɹ�����TRUE��ʧ�ܷ���FALSE
BOOL MemoryToFile(IN LPVOID pMemBuffer, IN size_t size, OUT LPCTSTR lpszFile)
{
	FILE* pFile = NULL;
	errno_t err = 0;
	err = _tfopen_s(&pFile, lpszFile, TEXT("wb+"));
	if (!pFile)
	{
		printf("(MemoryToFile)�ļ�����ʧ��,������룺%d\n", err);
		free(pMemBuffer);
		return FALSE;
	}

	fwrite(pMemBuffer, sizeof(BYTE), size, pFile);
	fclose(pFile);
	return TRUE;
}