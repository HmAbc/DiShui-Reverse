#include "ProcessList.h"

/// @brief 初始化ProcessListControl
/// @param hDlg 对话框句柄
VOID InitProcessListControl(HWND hDlg)
{
	LV_COLUMN lv;
	HWND hListProcess;
	TCHAR columnName[4][8] = { TEXT("进程"), TEXT("PID"), TEXT("镜像基址"), TEXT("镜像大小") };
	WORD columnWidth[4] = { 200, 100, 100 ,100 };

	//初始化
	memset(&lv, 0, sizeof(LV_COLUMN));
	//获取 IDC_LIST_PROCESS 句柄
	hListProcess = GetDlgItem(hDlg, IDC_LIST_PROCESS);
	//设置整行选中
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	//循环增加列
	for (size_t i = 0; i < 4; i++)
	{
		lv.pszText = columnName[i];		//列标题
		lv.cx = columnWidth[i];			//列宽度
		lv.iSubItem = i;

		//SendMessage(hListProcess, LVM_INSERTCOLUMN, i, (DWORD)&lv);
		ListView_InsertColumn(hListProcess, i, &lv);				//和SendMessage效果一样
	}
	EnumProcess(hListProcess);

}
