#pragma once
#include "Utils.h"

/// @brief ������ �Ի�����Ϣ������
/// @param hDlg �Ի�����
/// @param uMsg ��Ϣ���
/// @param wParam ��Ϣ��һ������
/// @param lParam ��Ϣ�ڶ�������
/// @return ������Ϣ����󷵻�TRUE��δ������FALSE������ϵͳ����
BOOL CALLBACK ExportDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
);

/// @brief ��ӡ����������
/// @param hDlg �Ի�����
/// @return �޴��󷵻�0���д��󷵻ش�����500+
DWORD InitExportTableDialog(IN HWND hDlg);
