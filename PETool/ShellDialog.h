#pragma once
#include "Utils.h"
#define KEY 0xD9876543

//#ifdef _UNICODE
//#define KEY 0xD987
//#else
//#define KEY 0x87
//#endif // _UNICODE

/// @brief �ӿ� �Ի�����Ϣ������
/// @param hDlg �Ի�����
/// @param uMsg ��Ϣ���
/// @param wParam ��Ϣ��һ������
/// @param lParam ��Ϣ�ڶ�������
/// @return ������Ϣ����󷵻�TRUE��δ������FALSE������ϵͳ����
BOOL CALLBACK ShellDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
);

/// @brief �ӿ��߳�
/// @param lpParameter �����߳�ʱ���ݵĲ��� 
/// @return ״̬��
DWORD WINAPI AddShell(LPVOID lpParameter);
