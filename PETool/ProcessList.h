#pragma once
#include "Utils.h"

/// @brief ��ʼ��ProcessListControl
VOID InitProcessListControl(HWND hDlg);

/// @brief ��ListView���������
/// @param hListProcess ListView�ľ��
BOOL EnumProcess(HWND hListProcess);

/// @brief ��ListView���������
/// @hInstance 
/// @param hListProcess ListView�ľ��
BOOL EnumModule(HWND hDlg, NM_LISTVIEW* pnmv);