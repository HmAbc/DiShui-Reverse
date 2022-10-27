#include "SectionDialog.h"

/// @brief 区段 对话框消息处理函数
/// @param hDlg 对话框句柄
/// @param uMsg 消息编号
/// @param wParam 消息第一个参数
/// @param lParam 消息第二个参数
/// @return 拦截消息处理后返回TRUE，未处理返回FALSE，交给系统处理
BOOL CALLBACK SectionDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		InitSectionDialog(hDlg);
		return TRUE;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

/// @brief 初始化 区段 对话框中的内容
/// @param hDlg 目录对话框的句柄
/// @return 无错误返回0，有错误返回错误码，400+
DWORD InitSectionDialog(IN HWND hDlg)
{
	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS ntHeader = NULL;
	PIMAGE_FILE_HEADER	peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;
	PIMAGE_SECTION_HEADER sectionHeader = NULL;
	LV_COLUMN lv;
	HWND sectionList;
	TCHAR columnName[5][10] = { TEXT("节名"), TEXT("文件偏移"), TEXT("文件大小"), TEXT("内存偏移"), TEXT("节区属性") };

	if (!fileBuffer)
	{
		DbgPrintf("no filebuffer");
		return 400;
	}

	dosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	ntHeader = (PIMAGE_NT_HEADERS)((DWORD)fileBuffer + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);
	sectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)peHeader + peHeader->SizeOfOptionalHeader);

	//初始化
	lv.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	memset(&lv, 0, sizeof(LV_COLUMN));
	//DbgPrintf("InitSectionDialog %u %d\n", GetLastError(), sectionList);
	sectionList = GetDlgItem(hDlg, IDC_LIST_SECTION);
	//设置整行选中效果
	SendMessage(sectionList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	//循环设置列名
	for (size_t i = 0; i < 5; i++)
	{
		lv.pszText = columnName[i];
		lv.cx = 75;
		lv.iSubItem = i;
		//SendMessage(sectionList, LVM_INSERTCOLUMN, i, (LPARAM)&lv);
		ListView_InsertColumn(sectionList, i, &lv);
	}
	DbgPrintf("InitSectionDialog %u %d\n", GetLastError(), sectionList);
	return 0;
}