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

/// @brief ���ý��̷���Ȩ�ޣ����ϵͳ����
/// @param lpName ��Ҫ��Ȩ�޵�����
/// @param opt ѡ���Ƿ�����Ȩ��
/// @return �ɹ�����TRUE
BOOL SetProcessPrivilege(PCWCHAR lpName, BOOL opt)
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
