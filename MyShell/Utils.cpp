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

/// @brief ����SizeOfImage��С���ڴ�imageBuffer����fileBuffer�����ļ����ݽڱ�������imageBuffer����
/// @param fileBuffer �ļ���������δ����״̬
/// @param imageBuffer �ļ��������ڴ���ʼ��ַ
/// @return �ɹ�����������СSizeOfImage��ʧ�ܷ���0
DWORD CopyFileBufferToImageBuffer(IN LPVOID fileBuffer, OUT LPVOID* imageBuffer)
{
	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS32 ntHeader = NULL;
	PIMAGE_FILE_HEADER peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER32 optionalHeader = NULL;
	PIMAGE_SECTION_HEADER sectionHeader = NULL;

	LPVOID imageBufferTemp = NULL;

	if (!fileBuffer)
	{
		return 0;
	}

	if (*((PWORD)fileBuffer) != IMAGE_DOS_SIGNATURE)
	{
		free(fileBuffer);
		return 0;
	}

	dosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	if (*((LPWORD)((DWORD)fileBuffer + dosHeader->e_lfanew)) != IMAGE_NT_SIGNATURE)
	{
		free(fileBuffer);
		return 0;
	}

	ntHeader = (PIMAGE_NT_HEADERS32)((DWORD)dosHeader + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER32)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);
	sectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)optionalHeader + peHeader->SizeOfOptionalHeader);

	imageBufferTemp = malloc(optionalHeader->SizeOfImage);
	if (!imageBufferTemp)
	{
		free(fileBuffer);
		return 0;
	}

	memset(imageBufferTemp, 0, optionalHeader->SizeOfImage);	//imageBufferTemp�ռ�
	memcpy(imageBufferTemp, dosHeader, optionalHeader->SizeOfHeaders);	//��dosHeader����optionalHeader->SizeOfHeaders��С�����ݵ�imageBuffer

	//ѭ��������
	PIMAGE_SECTION_HEADER pSectionTemp = sectionHeader;
	for (DWORD i = 0; i < peHeader->NumberOfSections; i++, pSectionTemp++)
	{
		memcpy((PDWORD)((DWORD)imageBufferTemp + pSectionTemp->VirtualAddress), (PDWORD)((DWORD)fileBuffer + pSectionTemp->PointerToRawData), pSectionTemp->SizeOfRawData);
	}

	*imageBuffer = imageBufferTemp;
	imageBufferTemp = NULL;

	return optionalHeader->SizeOfImage;
}

VOID GetProcessBase(IN LPVOID fileBuffer, OUT DWORD& imageBase, OUT DWORD& sizeOfImage, OUT DWORD& oep)
{
	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS ntHeader = NULL;
	PIMAGE_FILE_HEADER peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;

	dosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	ntHeader = (PIMAGE_NT_HEADERS)((DWORD)fileBuffer + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);

	imageBase = optionalHeader->ImageBase;
	sizeOfImage = optionalHeader->SizeOfImage;
	oep = optionalHeader->AddressOfEntryPoint;
}

BOOL IsRelocation(IN LPVOID fileBuffer)
{
	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS ntHeader = NULL;
	PIMAGE_FILE_HEADER peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;
	PIMAGE_DATA_DIRECTORY dataDir = NULL;

	dosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	ntHeader = (PIMAGE_NT_HEADERS)((DWORD)fileBuffer + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);
	dataDir = optionalHeader->DataDirectory;
	//�����ض�λ��
	if (dataDir[5].VirtualAddress)
	{
		return 1;
	}//������
	else
	{
		return 0;
	}
}

VOID RepaireRelocation(IN LPVOID imageBuffer, IN DWORD newImageBase)
{
	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS ntHeader = NULL;
	PIMAGE_FILE_HEADER peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;
	PIMAGE_DATA_DIRECTORY dataDir = NULL;
	PIMAGE_BASE_RELOCATION baseRelocation = NULL;

	dosHeader = (PIMAGE_DOS_HEADER)imageBuffer;
	ntHeader = (PIMAGE_NT_HEADERS)((DWORD)imageBuffer + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);
	dataDir = optionalHeader->DataDirectory;
	baseRelocation = (PIMAGE_BASE_RELOCATION)dataDir[5].VirtualAddress;

	DWORD hint = 0;
	DWORD tempRva = 0;
	DWORD rva = 0;
	PWORD address = NULL;
	PDWORD repairAddr = NULL;
	while (baseRelocation->VirtualAddress)
	{
		int count = (baseRelocation->SizeOfBlock - 8) / 2;
		address = (PWORD)((DWORD)baseRelocation + 8);
		for (int i = 0; i < count; i++)
		{
			hint = address[i] >> 12;
			if (hint == 3)
			{
				tempRva = address[i] & 0x0fff;
				rva = baseRelocation->VirtualAddress + tempRva;
				repairAddr = (PDWORD)((DWORD)imageBuffer + rva);
				*repairAddr = *repairAddr - optionalHeader->ImageBase + newImageBase;
			}
		}
		baseRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)baseRelocation + baseRelocation->SizeOfBlock);
	}
	//����Ϊ�µ�imagebase
	optionalHeader->ImageBase = newImageBase;
}