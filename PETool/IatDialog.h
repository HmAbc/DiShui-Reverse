#pragma once
#include "Utils.h"

/// @brief IAT �Ի�����Ϣ������
/// @param hDlg �Ի�����
/// @param uMsg ��Ϣ���
/// @param wParam ��Ϣ��һ������
/// @param lParam ��Ϣ�ڶ�������
/// @return ������Ϣ����󷵻�TRUE��δ������FALSE������ϵͳ����
BOOL CALLBACK IatDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
);

/// @brief ��ʼ��IAT��Ի����е�����
/// @param hDlg Ŀ¼�Ի���ľ��
/// @return �޴��󷵻�0���д��󷵻ش����룬1000+
DWORD InitIatDialog(IN HWND hDlg);