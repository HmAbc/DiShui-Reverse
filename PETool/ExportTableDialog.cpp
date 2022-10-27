#include "ExportTableDialog.h"

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
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		InitExportTableDialog(hDlg);
		return TRUE;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

/// @brief 打印导出表内容
/// @param hDlg 对话框句柄
/// @return 无错误返回0，有错误返回错误码500+
DWORD InitExportTableDialog(IN HWND hDlg)
{
	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS ntHeader = NULL;
	PIMAGE_FILE_HEADER	peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;
	PIMAGE_DATA_DIRECTORY dataDirectory = NULL;
	PIMAGE_EXPORT_DIRECTORY exportDirectory = NULL;
	HWND exportEdit;
	TCHAR tmpBuffer[20] = { 0 };

	if (!fileBuffer)
	{
		return 500;
	}

	dosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	ntHeader = (PIMAGE_NT_HEADERS)((DWORD)fileBuffer + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);

	exportEdit = GetDlgItem(hDlg, IDC_EDIT_EXPORTTABLE);

	dataDirectory = optionalHeader->DataDirectory;
	//没有导出表时
	if (!dataDirectory->VirtualAddress)
	{
		SendMessage(exportEdit, EM_REPLACESEL, TRUE, (LPARAM)TEXT("没有导出表"));
		return 501;
	}

	exportDirectory = (PIMAGE_EXPORT_DIRECTORY)(RVA2FOA(fileBuffer, dataDirectory->VirtualAddress) + (DWORD)fileBuffer);
	SendMessage(exportEdit, EM_REPLACESEL, TRUE, (LPARAM)TEXT("没有导出表"));
	

	return 0;
}