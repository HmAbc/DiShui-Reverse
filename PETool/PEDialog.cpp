#include "PEDialog.h"

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
)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			//按 区段 按钮时的逻辑
		case IDC_BUTTON_SECTION:

			return TRUE;
			//按 目录 按钮时的逻辑
		case IDC_BUTTON_DIRECTORY:

			return TRUE;
		default:
			break;
		}
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}