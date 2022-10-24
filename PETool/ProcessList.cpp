#include "ProcessList.h"
#include <psapi.h>

/// @brief 初始化ProcessListControl
/// @param hDlg 对话框句柄
VOID InitProcessListControl(HWND hDlg)
{
	LV_COLUMN lv;
	HWND hListProcess;
	TCHAR columnName[4][8] = { TEXT("进程"), TEXT("PID"), TEXT("镜像基址"), TEXT("镜像大小") };
	WORD columnWidth[4] = { 195, 100, 100 ,100 };

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


/// @brief 向ListView中添加数据
/// @param hListProcess ListView的句柄
BOOL EnumProcess(HWND hListProcess)
{
	LV_ITEM vItem;
	HANDLE hSnapshot, hModuleSnap;
	PROCESSENTRY32 processEntry;	//创建存放进程信息的结构体
	MODULEENTRY32 moduleEntry;		//创建存放模块信息的结构体
	BOOL isProcess = FALSE;
	INT rowNumber = 0;
	TCHAR* pid = new TCHAR[20]{ 0 };
	TCHAR* imageBase = new TCHAR[8]{ 0 };
	TCHAR* imageSize = new TCHAR[8]{ 0 };	

	//初始化
	memset(&vItem, 0, sizeof(LV_ITEM));
	vItem.mask = LVIF_TEXT;

	//创建进程快照
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	//初始化结构体
	processEntry.dwSize = sizeof(PROCESSENTRY32);
	moduleEntry.dwSize = sizeof(MODULEENTRY32);
	//从快照中读取第一个进程信息
	isProcess = Process32First(hSnapshot, &processEntry);
	while (isProcess)
	{
		//获取第一个模块，即程序自身，可以获得imageBase等
		hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, processEntry.th32ProcessID);
		if (hModuleSnap != INVALID_HANDLE_VALUE)
		{
			if (Module32First(hModuleSnap, &moduleEntry))
			{
				wsprintf(imageBase, TEXT("%#x"), (DWORD)(moduleEntry.modBaseAddr));
				wsprintf(imageSize, TEXT("%#x"), moduleEntry.modBaseSize);
			}
		}
		for (INT i = 0; i < 4; i++)
		{
			switch (i)
			{
			case 0:
				vItem.pszText = processEntry.szExeFile;
				break;
			case 1:
				wsprintf(pid, TEXT("%d"), processEntry.th32ProcessID);
				vItem.pszText = pid;
				break;
			case 2:
				vItem.pszText = imageBase;
				break;
			case 3:
				vItem.pszText = imageSize;
				break;
			default:
				break;
			}

			vItem.iItem = rowNumber;
			vItem.iSubItem = i;
			if (i == 0)
			{
				ListView_InsertItem(hListProcess, &vItem);		//第一列使用InsertItem
			}
			else
			{
				ListView_SetItem(hListProcess, &vItem);			//其他列使用SetItem
			}
		}
		isProcess = Process32Next(hSnapshot, &processEntry);
		rowNumber++;
		CloseHandle(hModuleSnap);
	}

	CloseHandle(hSnapshot);
	return TRUE;
}


BOOL EnumModule(HWND hDlg, NM_LISTVIEW* pnmv)
{
	LV_ITEM lv;
	HANDLE hModuleSnap, hProcess;
	HWND hListProcess, hListModule;
	MODULEENTRY32 moduleEntry;
	TCHAR pidBuffer[20] = { 0 };
	INT rowNumber = 0;
	INT pid = 0;
	BOOL isok = FALSE;

	//开启程序访问系统进程的权限
	isok = SetProcessPrivilege(SE_DEBUG_NAME, TRUE);

	//初始化
	moduleEntry.dwSize = sizeof(MODULEENTRY32);
	lv.mask = TVIF_TEXT;


	//获取IDC_LIST_PROCESS句柄
	hListProcess = GetDlgItem(hDlg, IDC_LIST_PROCESS);

	//获取点击IDC_LIST_PROCESS行的pid
	ListView_GetItemText(hListProcess, pnmv->iItem, 1, pidBuffer, 20);

	//获取IDC_LIST_MODULE句柄
	hListModule = GetDlgItem(hDlg, IDC_LIST_MODULE);

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _ttoi(pidBuffer));
	//根据pid创建进程模块快照
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, _ttoi(pidBuffer));

	DbgPrintf("%u %d %d\n", GetLastError(), (DWORD)hModuleSnap, (DWORD)hProcess);

	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		ListView_DeleteAllItems(hListModule);
		return 0;
	}

	if (Module32First(hModuleSnap, &moduleEntry))
	{
		do
		{
			//插入第一列模块名
			lv.pszText = moduleEntry.szModule;
			lv.iItem = rowNumber;
			lv.iSubItem = 0;
			ListView_InsertItem(hListModule, &lv);

			//插入第二列模块地址
			lv.pszText = moduleEntry.szExePath;
			lv.iItem = rowNumber;
			lv.iSubItem = 1;
			ListView_SetItem(hListModule, &lv);
		} while (Module32Next(hModuleSnap, &moduleEntry));
	}
	//关闭程序权限
	SetProcessPrivilege(SE_DEBUG_NAME, FALSE);
	CloseHandle(hModuleSnap);
	CloseHandle(hProcess);
	return TRUE;
}