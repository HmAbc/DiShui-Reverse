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
	DbgPrintf("%u\n", GetLastError());
	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS ntHeader = NULL;
	PIMAGE_FILE_HEADER	peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;
	PIMAGE_SECTION_HEADER sectionHeader = NULL;
	LV_COLUMN lv;
	LV_ITEM lvitem;
	HWND sectionList;
	TCHAR columnName[6][10] = { TEXT("节名"), TEXT("文件偏移"), TEXT("文件大小"), TEXT("内存偏移"), TEXT("内存大小"), TEXT("节区属性") };
	//SECTIONINFO sectionInfo;
	CHAR tmpName[9] = { 0 };
	TCHAR sectionInfo[6][9];

	if (!fileBuffer)
	{
		DbgPrintf("no filebuffer");
		return 400;
	}

	dosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	ntHeader = (PIMAGE_NT_HEADERS)((DWORD)fileBuffer + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);
	sectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)optionalHeader + peHeader->SizeOfOptionalHeader);

	//初始化
	memset(&lv, 0, sizeof(LV_COLUMN));
	lv.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

	sectionList = GetDlgItem(hDlg, IDC_LIST_SECTION);
	//设置整行选中效果
	SendMessage(sectionList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	//循环设置列名
	
	for (size_t i = 0; i < 6; i++)
	{
		lv.pszText = columnName[i];
		lv.cx = 100;
		lv.iSubItem = i;
		SendMessage(sectionList, LVM_INSERTCOLUMN, i, (LPARAM)&lv);
		//ListView_InsertColumn(sectionList, i, &lv);
	}

	//添加节信息
	memset(&lvitem, 0, sizeof(LV_ITEM));
	memset(&sectionInfo, 0, sizeof(sectionInfo));
	lvitem.mask = LVIF_TEXT;
	for (DWORD i = 0; i < peHeader->NumberOfSections; i++)
	{
		//节名
		memcpy(tmpName, sectionHeader[i].Name, 8);
		ANSIToUnicode(tmpName, sectionInfo[0]);
		//文件偏移
		wsprintf(sectionInfo[1], TEXT("%08X"), sectionHeader[i].PointerToRawData);
		//文件大小
		wsprintf(sectionInfo[2], TEXT("%08X"), sectionHeader[i].SizeOfRawData);
		//内存偏移
		wsprintf(sectionInfo[3], TEXT("%08X"), sectionHeader[i].VirtualAddress);
		//内存大小
		wsprintf(sectionInfo[4], TEXT("%08X"), sectionHeader[i].Misc.VirtualSize);
		//节区属性
		wsprintf(sectionInfo[5], TEXT("%08X"), sectionHeader[i].Characteristics);
		for (size_t j = 0; j < 6; j++)
		{
			lvitem.pszText = sectionInfo[j];
			lvitem.iItem = i;
			lvitem.iSubItem = j;
			
			if (j == 0)
			{
				SendMessage(sectionList, LVM_INSERTITEM, 0, (LPARAM)&lvitem);
			}
			else
			{
				SendMessage(sectionList, LVM_SETITEM, 0, (LPARAM)&lvitem);
			}
		}			
	}

	return 0;
}
