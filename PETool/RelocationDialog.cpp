#include "RelocationDialog.h"

/// @brief 重定位表 对话框消息处理函数
/// @param hDlg 对话框句柄
/// @param uMsg 消息编号
/// @param wParam 消息第一个参数
/// @param lParam 消息第二个参数
/// @return 拦截消息处理后返回TRUE，未处理返回FALSE，交给系统处理
BOOL CALLBACK RelocationDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		InitRelocationDialog(hDlg);
		return TRUE;

	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}


/// @brief 初始化重定位表对话框中的内容
/// @param hDlg 目录对话框的句柄
/// @return 无错误返回0，有错误返回错误码，800+
DWORD InitRelocationDialog(IN HWND hDlg)
{
	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS ntHeader = NULL;
	PIMAGE_FILE_HEADER peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;
	PIMAGE_DATA_DIRECTORY dataDirectory = NULL;
	PIMAGE_BASE_RELOCATION baseRelocation = NULL;
	HWND relocationEdit;
	TCHAR tempBuffer[80] = { 0 };
	//std::tstring* contentBuffer = new std::tstring();
	std::tstring contentBuffer;
	std::tstring tempStr;
	WORD index = 1;
	PWORD relocationAddr = NULL;
	DWORD relocationSize = 0;
	DWORD numberOfItem = 0;

	if (!fileBuffer)
	{
		return 800;
	}

	dosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	ntHeader = (PIMAGE_NT_HEADERS)((DWORD)fileBuffer + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);
	//获取编辑框句柄
	relocationEdit = GetDlgItem(hDlg, IDC_EDIT_RELOCATION);

	dataDirectory = optionalHeader->DataDirectory;
	//没有重定位表
	if (!dataDirectory[5].VirtualAddress)
	{
		contentBuffer += TEXT("\t没有重定位表\r\n");
		SendMessage(relocationEdit, EM_REPLACESEL, FALSE, (LPARAM)contentBuffer.c_str());
		return 801;
	}

	baseRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)RVA2FOA(fileBuffer, dataDirectory[5].VirtualAddress) + (DWORD)fileBuffer);
	contentBuffer += TEXT("RelocationTable:\r\n");

	while (baseRelocation->VirtualAddress)
	{
		relocationAddr = (PWORD)((DWORD)baseRelocation + 8);
		relocationSize = baseRelocation->SizeOfBlock;

		wsprintf(tempBuffer, TEXT("\r\n  %d. 重定位块：\r\n\tVirtualAddress:\t%08X\r\n\tSizeOfBlock:\t%08X\r\n\r\n"), 
			index, baseRelocation->VirtualAddress, relocationSize);
		tempStr = tempBuffer;
		contentBuffer += tempStr;
		contentBuffer += TEXT("\t地址\t  属性\r\n\t———————— ———————\r\n");

		numberOfItem = (relocationSize - 8) / 2;
		for (DWORD i = 0; i < numberOfItem; i++)
		{
			wsprintf(tempBuffer, TEXT("\t%08X    %d\r\n"), relocationAddr[i] & 0xFFF + baseRelocation->VirtualAddress, (relocationAddr[i] >> 12) & 0xF);
			tempStr = tempBuffer;
			contentBuffer += tempStr;
		}

		index++;
		baseRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)baseRelocation + relocationSize);
	}
	
	SendMessage(relocationEdit, EM_REPLACESEL, FALSE, (LPARAM)contentBuffer.c_str());

	return 0;
}