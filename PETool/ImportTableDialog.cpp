#include "ImportTableDialog.h"

/// @brief 导入表 对话框消息处理函数
/// @param hDlg 对话框句柄
/// @param uMsg 消息编号
/// @param wParam 消息第一个参数
/// @param lParam 消息第二个参数
/// @return 拦截消息处理后返回TRUE，未处理返回FALSE，交给系统处理
BOOL CALLBACK ImportDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		InitImportDialog(hDlg);
		return TRUE;

	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

/// @brief 初始化导入表对话框中的内容
/// @param hDlg 目录对话框的句柄
/// @return 无错误返回0，有错误返回错误码，600+
DWORD InitImportDialog(IN HWND hDlg)
{
	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS ntHeader = NULL;
	PIMAGE_FILE_HEADER	peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;
	PIMAGE_DATA_DIRECTORY dataDirectory = NULL;
	PIMAGE_IMPORT_DESCRIPTOR importDescriptor = NULL;
	PDWORD originalFirstThunk;
	PIMAGE_IMPORT_BY_NAME importByName;
	HWND importEdit;
	TCHAR tempBuffer[60] = { 0 };
	TCHAR tempNameBuffer[40] = { 0 };
	std::tstring contentBuffer;
	std::tstring tempStr;
	INT index = 1;
	DWORD tempNameAddr = 0;

	if (!fileBuffer)
	{
		return 600;
	}

	dosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	ntHeader = (PIMAGE_NT_HEADERS)((DWORD)fileBuffer + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);

	importEdit = GetDlgItem(hDlg, IDC_EDIT_IMPORTTABLE);

	dataDirectory = optionalHeader->DataDirectory;
	//没有导出表时
	if (!dataDirectory[1].VirtualAddress)
	{
		contentBuffer += TEXT("\t没有导入表\r\n");
		SendMessage(importEdit, EM_REPLACESEL, TRUE, (LPARAM)contentBuffer.c_str());
		return 601;
	}

	importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(RVA2FOA(fileBuffer, dataDirectory[1].VirtualAddress) + (DWORD)fileBuffer);
	contentBuffer += TEXT("ImportTable:\r\n");

	while (importDescriptor->OriginalFirstThunk)
	{
		contentBuffer += TEXT("\r\n-------------------------------------------\r\n");

		//处理DLL名字
		tempNameAddr = RVA2FOA(fileBuffer, importDescriptor->Name) + (DWORD)fileBuffer;
		ANSIToUnicode((PCSTR)tempNameAddr, tempNameBuffer);
		wsprintf(tempBuffer, TEXT("\t%d、%s\r\n"), index, tempNameBuffer);
		tempStr = tempBuffer;
		contentBuffer += tempStr;

		//处理 OriginalFirstThunk
		originalFirstThunk = (PDWORD)(RVA2FOA(fileBuffer, importDescriptor->OriginalFirstThunk) + (DWORD)fileBuffer);
		contentBuffer += TEXT("\t\tOriginalFirstThunk:\r\n");

		while (*originalFirstThunk)
		{
			//以序号导入
			if (*originalFirstThunk & IMAGE_ORDINAL_FLAG)
			{
				wsprintf(tempBuffer, TEXT("\t\t\t以序号导入：%d\r\n"), *originalFirstThunk & 0x7FFFFFFF);
			}
			else  //按名字导入
			{
				importByName = (PIMAGE_IMPORT_BY_NAME)(RVA2FOA(fileBuffer, *originalFirstThunk) + (DWORD)fileBuffer);
				//处理名字
				ANSIToUnicode((PCSTR)importByName->Name, tempNameBuffer);
				wsprintf(tempBuffer, TEXT("\t\t\t以名字导入：HINT %X，%s\r\n"), importByName->Hint, tempNameBuffer);
				tempStr = tempBuffer;

				contentBuffer += tempStr;
			}
			originalFirstThunk++;
		}
		index++;
		importDescriptor++;
	}

	SendMessage(importEdit, EM_REPLACESEL, TRUE, (LPARAM)contentBuffer.c_str());

	return 0;
}
