#pragma once
#include "Utils.h"

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
