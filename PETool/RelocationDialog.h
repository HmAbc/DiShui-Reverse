#pragma once
#include "Utils.h"

/// @brief ��Դ�� �Ի�����Ϣ������
/// @param hDlg �Ի�����
/// @param uMsg ��Ϣ���
/// @param wParam ��Ϣ��һ������
/// @param lParam ��Ϣ�ڶ�������
/// @return ������Ϣ����󷵻�TRUE��δ������FALSE������ϵͳ����
BOOL CALLBACK RelocationDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
);

/// @brief ��ʼ���ض�λ��Ի����е�����
/// @param hDlg Ŀ¼�Ի���ľ��
/// @return �޴��󷵻�0���д��󷵻ش����룬800+
DWORD InitRelocationDialog(IN HWND hDlg);