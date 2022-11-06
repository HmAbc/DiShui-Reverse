#define _CRT_SECURE_NO_WARNINGS
#pragma once

#include <Windows.h>
#include "resource.h"
#include <stdio.h>

void __cdecl OutputDebugStringF(const char* format, ...);

#ifdef _DEBUG  
#define DbgPrintf   OutputDebugStringF  
#else  
#define DbgPrintf  
#endif 

/// @brief �� �Ի�����Ϣ������
/// @param hDlg �Ի�����
/// @param uMsg ��Ϣ���
/// @param wParam ��Ϣ��һ������
/// @param lParam ��Ϣ�ڶ�������
/// @return ������Ϣ����󷵻�TRUE��δ������FALSE������ϵͳ����
BOOL CALLBACK MainDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
);
