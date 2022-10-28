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
	TCHAR tempBuffer[60] = { 0 };
	TCHAR tempNameBuffer[20] = { 0 };
	std::tstring contentBuffer;
	std::tstring tempStr;
	BOOL flag = 0;
	PDWORD functionAddress;
	PWORD exportNumber;
	PDWORD nameAdderss;
	DWORD tempNameAddr = 0;

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
		contentBuffer += TEXT("\t没有导出表\r\n");
		SendMessage(exportEdit, EM_REPLACESEL, TRUE, (LPARAM)contentBuffer.c_str());
		return 501;
	}
	//导出表FOA
	exportDirectory = (PIMAGE_EXPORT_DIRECTORY)(RVA2FOA(fileBuffer, dataDirectory->VirtualAddress) + (DWORD)fileBuffer);
	//函数地址FOA
	functionAddress = (PDWORD)(RVA2FOA(fileBuffer, exportDirectory->AddressOfFunctions) + (DWORD)fileBuffer);
	//导出序号FOA
	exportNumber = (PWORD)(RVA2FOA(fileBuffer, exportDirectory->AddressOfNameOrdinals) + (DWORD)fileBuffer);
	//函数名称FOA
	nameAdderss = (PDWORD)(RVA2FOA(fileBuffer, exportDirectory->AddressOfNames) + (DWORD)fileBuffer);

	contentBuffer += TEXT("ExportTable：\r\n");
	wsprintf(tempBuffer, TEXT("\tCharacteristics：\t%08X\r\n"), exportDirectory->Characteristics);
	tempStr = tempBuffer;
	contentBuffer += tempStr;
	wsprintf(tempBuffer, TEXT("\tTimeDataStamp：\t%08X\r\n"), exportDirectory->TimeDateStamp);
	tempStr = tempBuffer;
	contentBuffer += tempStr;
	wsprintf(tempBuffer, TEXT("\tMajorVersion：\t%04X\r\n"), exportDirectory->MajorVersion);
	tempStr = tempBuffer;
	contentBuffer += tempStr;
	wsprintf(tempBuffer, TEXT("\tMinorVersion：\t%04X\r\n"), exportDirectory->MinorVersion);
	tempStr = tempBuffer;
	contentBuffer += tempStr;
	wsprintf(tempBuffer, TEXT("\tName：\t\t%08X\r\n"), exportDirectory->Name);
	tempStr = tempBuffer;
	contentBuffer += tempStr;
	wsprintf(tempBuffer, TEXT("\tBase：\t\t%08X\r\n"), exportDirectory->Base);
	tempStr = tempBuffer;
	contentBuffer += tempStr;
	wsprintf(tempBuffer, TEXT("\tNumberOfFunctions：\t%08X\r\n"), exportDirectory->NumberOfFunctions);
	tempStr = tempBuffer;
	contentBuffer += tempStr;
	wsprintf(tempBuffer, TEXT("\tNumberOfNames：\t\t%08X\r\n"), exportDirectory->NumberOfNames);
	tempStr = tempBuffer;
	contentBuffer += tempStr;
	wsprintf(tempBuffer, TEXT("\tAddressOfFunctions：\t%08X\r\n"), exportDirectory->AddressOfFunctions);
	tempStr = tempBuffer;
	contentBuffer += tempStr;
	wsprintf(tempBuffer, TEXT("\tAddressOfNames：\t\t%08X\r\n"), exportDirectory->AddressOfNames);
	tempStr = tempBuffer;
	contentBuffer += tempStr;
	wsprintf(tempBuffer, TEXT("\tAddressOfNameOrdinals：\t%08X\r\n\r\n"), exportDirectory->AddressOfNameOrdinals);
	tempStr = tempBuffer;
	contentBuffer += tempStr;

	contentBuffer += TEXT("函数地址\t导出序号\t函数名称\r\n");

	for (DWORD i = 0; i < exportDirectory->NumberOfFunctions; i++)
	{
		for (DWORD j = 0; j < exportDirectory->NumberOfNames; j++)
		{
			if (i == exportNumber[j])
			{
				//处理函数名字，从ANSCII码字符串转为UNICODE字符串
				tempNameAddr = RVA2FOA(fileBuffer, nameAdderss[j]) + (DWORD)fileBuffer;
				ANSIToUnicode((PCSTR)tempNameAddr, tempNameBuffer);

				wsprintf(tempBuffer, TEXT("%08X\t%04d\t\t%s\r\n"), functionAddress[i], exportNumber[j] + exportDirectory->Base, tempNameBuffer);
				tempStr = tempBuffer;
				contentBuffer += tempStr;
				flag = 1;
			}
		}
		if (!flag)
		{
			contentBuffer += std::to_tstring(functionAddress[i]) + TEXT("\r\n");
		}
		flag = 0;
	}

	SendMessage(exportEdit, EM_REPLACESEL, TRUE, (LPARAM)contentBuffer.c_str());

	return 0;
}