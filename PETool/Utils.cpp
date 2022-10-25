#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include "Utils.h"

extern HINSTANCE hAppInstance;

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
BOOL SetProcessPrivilege(IN PCWCHAR lpName, IN BOOL opt)
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
	if ((()(DWORD)fileBufferTemp + 4))
	{

	}
}