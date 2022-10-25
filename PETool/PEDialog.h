#pragma once
#include "Utils.h"

/// @brief 定义PE信息查看对话框消息处理回调函数
/// @param hwnd 窗口句柄
/// @param uMsg 消息类型
/// @param wParam 第一个消息参数
/// @param lParam 第二个消息参数
/// @return 处理完消息返回TRUE，未处理的消息会交给系统处理
BOOL CALLBACK PEDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
);

/// @brief 输入PE文件路径，在PE信息查看dialog中显示信息
/// @param filePath PE文件路径
DWORD InitPEImformation(IN LPCTSTR filePath, IN HWND hDlg);
