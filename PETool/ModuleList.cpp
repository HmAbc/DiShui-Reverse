#include "ModuleList.h"


/// @brief 初始化ModuleListControl
/// @param hDlg 对话框句柄
VOID InitModuleListControl(HWND hDlg)
{
	LV_COLUMN lv;
	HWND hListProcess;
	TCHAR columnName[2][8] = { TEXT("模块名称"),TEXT("模块位置") };
	WORD columnWidth[2] = { 200, 300 };

	memset(&lv, 0, sizeof(LV_COLUMN));
	hListProcess = GetDlgItem(hDlg, IDC_LIST_MODULE);
	//可以选中整行
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	//循环设置列名
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	for (size_t i = 0; i < 2; i++)
	{
		lv.pszText = columnName[i];
		lv.cx = columnWidth[i];
		lv.iSubItem = i;
		ListView_InsertColumn(hListProcess, i, &lv);
	}
}
