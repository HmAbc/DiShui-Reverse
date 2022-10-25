#pragma once
#include "Utils.h"

/// @brief ����PE��Ϣ�鿴�Ի�����Ϣ����ص�����
/// @param hwnd ���ھ��
/// @param uMsg ��Ϣ����
/// @param wParam ��һ����Ϣ����
/// @param lParam �ڶ�����Ϣ����
/// @return ��������Ϣ����TRUE��δ�������Ϣ�ύ��ϵͳ����
BOOL CALLBACK PEDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
);

/// @brief ����PE�ļ�·������PE��Ϣ�鿴dialog����ʾ��Ϣ
/// @param filePath PE�ļ�·��
DWORD InitPEImformation(IN LPCTSTR filePath, IN HWND hDlg);
