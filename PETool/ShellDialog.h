#pragma once
#include "Utils.h"
#define KEY 0xD9876543

//#ifdef _UNICODE
//#define KEY 0xD987
//#else
//#define KEY 0x87
//#endif // _UNICODE

/// @brief 加壳 对话框消息处理函数
/// @param hDlg 对话框句柄
/// @param uMsg 消息编号
/// @param wParam 消息第一个参数
/// @param lParam 消息第二个参数
/// @return 拦截消息处理后返回TRUE，未处理返回FALSE，交给系统处理
BOOL CALLBACK ShellDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
);

/// @brief 加壳线程
/// @param lpParameter 创建线程时传递的参数 
/// @return 状态码
DWORD WINAPI AddShell(LPVOID lpParameter);
