#pragma once
#include "Utils.h"

/// @brief ����DIALOG��Ϣ����ص�����
/// @param hwnd ���ھ��
/// @param uMsg ��Ϣ����
/// @param wParam ��һ����Ϣ����
/// @param lParam �ڶ�����Ϣ����
/// @return ��������Ϣ����TRUE
BOOL CALLBACK MainDialogProc(
	HWND hwnd,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
);


/// @brief ��ʼ��ModuleListControl
/// @param hDlg �Ի�����
VOID InitModuleListControl(HWND hDlg);

/// @brief ��ʼ��ProcessListControl
VOID InitProcessListControl(HWND hDlg);

/// @brief ��ListView���������
/// @param hListProcess ListView�ľ��
BOOL EnumProcess(HWND hListProcess);

/// @brief ��ListView���������
/// @hInstance 
/// @param hListProcess ListView�ľ��
BOOL EnumModule(HWND hDlg, NM_LISTVIEW* pnmv);