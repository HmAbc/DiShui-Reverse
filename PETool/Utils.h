#pragma once
#include <Windows.h>
#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#include <stdio.h>
#include "resource.h"
#include <Tlhelp32.h>

void __cdecl OutputDebugStringF(const char* format, ...);

#ifdef _DEBUG  
#define DbgPrintf   OutputDebugStringF  
#else  
#define DbgPrintf  
#endif 


/// @brief ����DIALOG��Ϣ����ص�����
/// @param hwnd ���ھ��
/// @param uMsg ��Ϣ����
/// @param wParam ��һ����Ϣ����
/// @param lParam �ڶ�����Ϣ����
/// @return ��������Ϣ����TRUE
BOOL CALLBACK MainDialogProc(
	HWND hwnd,
	// handle of window
	UINT uMsg,
	// message identifier
	WPARAM wParam,
	// first message parameter
	LPARAM lParam
	// second message parameter
);

/// @brief ��ListView���������
/// @param hListProcess ListView�ľ��
BOOL EnumProcess(HWND hListProcess);
