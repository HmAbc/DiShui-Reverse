#include "IatDialog.h"

/// @brief IAT 对话框消息处理函数
/// @param hDlg 对话框句柄
/// @param uMsg 消息编号
/// @param wParam 消息第一个参数
/// @param lParam 消息第二个参数
/// @return 拦截消息处理后返回TRUE，未处理返回FALSE，交给系统处理
BOOL CALLBACK IatDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		InitIatDialog(hDlg);
		return TRUE;

	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;

	default:
		break;
	}
	return FALSE;
}

/// @brief 初始化IAT表对话框中的内容
/// @param hDlg 目录对话框的句柄
/// @return 无错误返回0，有错误返回错误码，1000+
DWORD InitIatDialog(IN HWND hDlg)
{
	HWND iatEdit = GetDlgItem(hDlg, IDC_EDIT_IAT);

	SendMessage(iatEdit, EM_REPLACESEL, FALSE, (LPARAM)TEXT("  累了，不写了"));
	return 0;
}