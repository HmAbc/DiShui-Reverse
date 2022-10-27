#pragma once
#include "Utils.h"

/// @brief 定义DIALOG消息处理回调函数
/// @param hwnd 窗口句柄
/// @param uMsg 消息类型
/// @param wParam 第一个消息参数
/// @param lParam 第二个消息参数
/// @return 处理完消息返回TRUE
BOOL CALLBACK MainDialogProc(
	HWND hwnd,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
);


/// @brief 初始化ModuleListControl
/// @param hDlg 对话框句柄
VOID InitModuleListControl(HWND hDlg);

/// @brief 初始化ProcessListControl
VOID InitProcessListControl(HWND hDlg);

/// @brief 向ListView中添加数据
/// @param hListProcess ListView的句柄
BOOL EnumProcess(HWND hListProcess);

/// @brief 向ListView中添加数据
/// @hInstance 
/// @param hListProcess ListView的句柄
BOOL EnumModule(HWND hDlg, NM_LISTVIEW* pnmv);