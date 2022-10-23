#pragma once
#include "Utils.h"

/// @brief 初始化ProcessListControl
VOID InitProcessListControl(HWND hDlg);

/// @brief 向ListView中添加数据
/// @param hListProcess ListView的句柄
BOOL EnumProcess(HWND hListProcess);

/// @brief 向ListView中添加数据
/// @hInstance 
/// @param hListProcess ListView的句柄
BOOL EnumModule(HWND hDlg, NM_LISTVIEW* pnmv);