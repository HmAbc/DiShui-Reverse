#include "DirectoryDialog.h"

/// @brief 目录 对话框消息处理函数
/// @param hDlg 对话框句柄
/// @param uMsg 消息编号
/// @param wParam 消息第一个参数
/// @param lParam 消息第二个参数
/// @return 拦截消息处理后返回TRUE，未处理返回FALSE，交给系统处理
BOOL CALLBACK DirectoryDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:;
		InitDirectoryDialog(hDlg);
		DbgPrintf("InitDirectoryDialog %u\n", GetLastError());
		return TRUE;

	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}


/// @brief 初始化目录对话框中的内容
/// @param hDlg 目录对话框的句柄
/// @return 无错误返回0，有错误返回错误码300+
DWORD InitDirectoryDialog(IN HWND hDlg)
{
	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS ntHeader = NULL;
	PIMAGE_FILE_HEADER	peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;
	PIMAGE_DATA_DIRECTORY dataDirectory = NULL;

	INT rva[16] = { IDC_EDIT_RVA1, IDC_EDIT_RVA2, IDC_EDIT_RVA3, IDC_EDIT_RVA4, IDC_EDIT_RVA5, 
		IDC_EDIT_RVA6, IDC_EDIT_RVA7, IDC_EDIT_RVA8, IDC_EDIT_RVA9, IDC_EDIT_RVA10, IDC_EDIT_RVA11, 
		IDC_EDIT_RVA12, IDC_EDIT_RVA13, IDC_EDIT_RVA14, IDC_EDIT_RVA15, IDC_EDIT_RVA16 };
	INT size[16] = { IDC_EDIT_SIZE1, IDC_EDIT_SIZE2, IDC_EDIT_SIZE3, IDC_EDIT_SIZE4, IDC_EDIT_SIZE5,
		IDC_EDIT_SIZE6, IDC_EDIT_SIZE7, IDC_EDIT_SIZE8, IDC_EDIT_SIZE9, IDC_EDIT_SIZE10, IDC_EDIT_SIZE11,
		IDC_EDIT_SIZE12, IDC_EDIT_SIZE13, IDC_EDIT_SIZE14, IDC_EDIT_SIZE15, IDC_EDIT_SIZE16 };
	TCHAR temp[20] = { 0 };
	//TCHAR rvaTemp[20] = TEXT("IDC_EDIT_RVA");
	//TCHAR sizeTemp[20] = TEXT("IDC_EDIT_SIZE");
	//TCHAR index[3] = { 0 };

	//fileSize = ReadPEFile(filePath, &fileBuffer);
	if (!fileBuffer)
	{
		return 300;
	}

	dosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	ntHeader = (PIMAGE_NT_HEADERS)((DWORD)fileBuffer + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);

	dataDirectory = optionalHeader->DataDirectory;

	for (DWORD i = 0; i < 16; i++)
	{
		//设置rva
		wsprintf(temp, TEXT("%08X"), dataDirectory[i].VirtualAddress);
		SendDlgItemMessage(hDlg, rva[i], WM_SETTEXT, 0, (LPARAM)temp);

		//设置size
		wsprintf(temp, TEXT("%08X"), dataDirectory[i].Size);
		SendDlgItemMessage(hDlg, size[i], WM_SETTEXT, 0, (LPARAM)temp);

	}

	return 0;
}