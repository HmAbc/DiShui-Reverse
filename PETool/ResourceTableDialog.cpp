#include "ResourceTableDialog.h"

/// @brief 资源表 对话框消息处理函数
/// @param hDlg 对话框句柄
/// @param uMsg 消息编号
/// @param wParam 消息第一个参数
/// @param lParam 消息第二个参数
/// @return 拦截消息处理后返回TRUE，未处理返回FALSE，交给系统处理
BOOL CALLBACK ResourceDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		InitResourceDialog(hDlg);
		return TRUE;

	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}


/// @brief 初始化资源表对话框中的内容
/// @param hDlg 目录对话框的句柄
/// @return 无错误返回0，有错误返回错误码，700+
DWORD InitResourceDialog(IN HWND hDlg)
{
	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS ntHeader = NULL;
	PIMAGE_FILE_HEADER peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;
	PIMAGE_DATA_DIRECTORY dataDirectory = NULL;
	PIMAGE_RESOURCE_DIRECTORY resourceDir = NULL;
	PIMAGE_RESOURCE_DIRECTORY resourceDirSecond = NULL;
	PIMAGE_RESOURCE_DIRECTORY resourceDirThird = NULL;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY resourceDirEntry = NULL;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY resourceDirEntrySec = NULL;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY resourceDirEntryThird = NULL;
	PIMAGE_RESOURCE_DIR_STRING_U resourceDirStringU = NULL;
	HWND resourceEdit;
	TCHAR tempBuffer[40] = { 0 };
	TCHAR tempNameBuffer[20] = { 0 };
	std::tstring contentBuffer;
	std::tstring tempStr;
	
	TCHAR resourceType[17][8] = { 
		TEXT(""), TEXT("光标"), TEXT("位图"), TEXT("图标"), TEXT("菜单"), TEXT("对话框"), 
		TEXT("字符串"), TEXT("字体目录"), TEXT("字体"), TEXT("加速键"), TEXT("未格式化资源"), TEXT("消息表"), 
		TEXT("光标组"), TEXT(""), TEXT("图标组"), TEXT(""), TEXT("版本信息") };

	if (!fileBuffer)
	{
		return 700;
	}

	dosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	ntHeader = (PIMAGE_NT_HEADERS)((DWORD)fileBuffer + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);

	dataDirectory = optionalHeader->DataDirectory;
	DWORD t = RVA2FOA(fileBuffer, dataDirectory[2].VirtualAddress);
	resourceDir = (PIMAGE_RESOURCE_DIRECTORY)(t + (DWORD)fileBuffer);
	resourceDirEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)resourceDir + 16);

	contentBuffer += TEXT("ResourceTable:\r\n");

	//处理资源表第一层，资源类型或资源ID
	for (WORD i = 0; i < resourceDir->NumberOfIdEntries + resourceDir->NumberOfNamedEntries; i++)
	{
		contentBuffer += TEXT("\r\n----------------------------------------------------------\r\n");
		if (resourceDirEntry[i].Name > 16)
		{
			wsprintf(tempBuffer, TEXT("第%d个资源类型：%s\r\n"), i + 1, TEXT("自定义类型"));
			tempStr = tempBuffer;
			contentBuffer += tempStr;
		}
		else
		{
			wsprintf(tempBuffer, TEXT("第%d个资源类型：%s\r\n"), i + 1, resourceType[resourceDirEntry[i].Name]);
			tempStr = tempBuffer;
			contentBuffer += tempStr;
		}
		wsprintf(tempBuffer, TEXT("offset：%X\r\n"), resourceDirEntry[i].OffsetToDirectory);
		tempStr = tempBuffer;
		contentBuffer += tempStr;

		//处理资源表第二层
		//第二层IMAGE_RESOURCE_DIRECTORY
		resourceDirSecond = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)resourceDir + resourceDirEntry[i].OffsetToDirectory);
		resourceDirEntrySec = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)resourceDirSecond + 16);
		for (WORD i = 0; i < resourceDirSecond->NumberOfIdEntries + resourceDirSecond->NumberOfNamedEntries; i++)
		{
			//判断是否是名称
			if (resourceDirEntrySec[i].NameIsString == 1)
			{
				resourceDirStringU = (PIMAGE_RESOURCE_DIR_STRING_U)((DWORD)resourceDir + resourceDirEntrySec[i].NameOffset);
				//读取名称，宽字符，pResourceDirStringU->NameString是字符串地址，不需要继续转换
				_tcsncpy_s(tempBuffer, 40, resourceDirStringU->NameString, resourceDirStringU->Length);
				//tempBuffer[resourceDirStringU->Length + 1] = '\0';
				tempStr = tempBuffer;
				contentBuffer += TEXT("\t资源名称：") + tempStr + TEXT("\r\n");
			}
			else
			{
				wsprintf(tempBuffer, TEXT("\t资源ID：%d\r\n"), resourceDirEntrySec[i].Id);
				tempStr = tempBuffer;
				contentBuffer += tempStr;
			}
			//处理资源表第三层，只打印RVA和SIZE
			resourceDirThird = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)resourceDir + resourceDirEntrySec[i].OffsetToDirectory);
			resourceDirEntryThird = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)resourceDirThird + 16);
			dataDirectory = (PIMAGE_DATA_DIRECTORY)((DWORD)resourceDir + resourceDirEntryThird->OffsetToData);

			wsprintf(tempBuffer, TEXT("\t\tRVA：%X,  SIZE：%X\r\n"), dataDirectory->VirtualAddress, dataDirectory->Size);
			tempStr = tempBuffer;
			contentBuffer += tempStr;
		}
	}

	resourceEdit = GetDlgItem(hDlg, IDC_EDIT_RESOURCETABLE);
	SendMessage(resourceEdit, EM_REPLACESEL, TRUE, (LPARAM)contentBuffer.c_str());

	return 0;
}
