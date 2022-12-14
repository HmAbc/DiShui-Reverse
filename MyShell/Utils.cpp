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

/// @brief 申请SizeOfImage大小的内存imageBuffer，从fileBuffer处将文件根据节表拷贝到imageBuffer处，
/// @param fileBuffer 文件缓冲区，未拉伸状态
/// @param imageBuffer 文件拉伸后的内存起始地址
/// @return 成功返回拉伸后大小SizeOfImage，失败返回0
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

	memset(imageBufferTemp, 0, optionalHeader->SizeOfImage);	//imageBufferTemp空间
	memcpy(imageBufferTemp, dosHeader, optionalHeader->SizeOfHeaders);	//从dosHeader拷贝optionalHeader->SizeOfHeaders大小的数据到imageBuffer

	//循环拷贝节
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
	//存在重定位表
	if (dataDir[5].VirtualAddress)
	{
		return 1;
	}//不存在
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
	//更改为新的imagebase
	optionalHeader->ImageBase = newImageBase;
}