#pragma once
#include "Utils.h"

/// @brief 导出表 对话框消息处理函数
/// @param hDlg 对话框句柄
/// @param uMsg 消息编号
/// @param wParam 消息第一个参数
/// @param lParam 消息第二个参数
/// @return 拦截消息处理后返回TRUE，未处理返回FALSE，交给系统处理
BOOL CALLBACK ExportDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
);

/// @brief 打印导出表内容
/// @param hDlg 对话框句柄
/// @return 无错误返回0，有错误返回错误码500+
DWORD InitExportTableDialog(IN HWND hDlg);
