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
BOOL SetProcessPrivilege(IN PCWCHAR lpName, IN BOOL opt)
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
	PVOID fileBufferTemp = NULL;
	PIMAGE_DOS_HEADER dosHeader;


	_tfopen_s(&file, filePath, TEXT("rb"));
	if (!file)
	{
		return 100;
	}

	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	fileBufferTemp = malloc(fileSize);
	if (!fileBufferTemp)
	{
		fclose(file);
		return 101;
	}

	DWORD n = fread(fileBufferTemp, sizeof(BYTE), fileSize, file);
	if (!n)
	{
		fclose(file);
		free(fileBufferTemp);
		return 102;
	}

	if (*(PWORD)fileBufferTemp != IMAGE_DOS_SIGNATURE)
	{
		fclose(file);
		free(fileBufferTemp);
		return 103;
	}

	dosHeader = (PIMAGE_DOS_HEADER)fileBufferTemp;
	if (*(PDWORD)((DWORD)fileBufferTemp + dosHeader->e_lfanew) != IMAGE_NT_SIGNATURE)
	{
		fclose(file);
		free(fileBufferTemp);
		return 104;
	}

	*fileBuffer = fileBufferTemp;
	fileBufferTemp = NULL;
	fclose(file);
	return 0;
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
		maxSize = sectionHeader[i].Misc.VirtualSize > sectionHeader->SizeOfRawData ? sectionHeader[i].Misc.VirtualSize : sectionHeader->SizeOfRawData;
		if (sectionHeader[i].VirtualAddress + maxSize > rva)
		{
			return rva - sectionHeader[i].VirtualAddress + sectionHeader[i].PointerToRawData;
		}
	}

	return 0;
}