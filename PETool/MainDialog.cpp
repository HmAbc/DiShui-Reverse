#include "MainDialog.h"
#include "About.h"
#include "PEDialog.h"

//HINSTANCE hAppInstance;


/// @brief 定义MAIN对话框消息处理回调函数
/// @param hwnd 窗口句柄
/// @param uMsg 消息类型
/// @param wParam 第一个消息参数
/// @param lParam 第二个消息参数
/// @return 处理完消息返回TRUE，未处理的消息会交给系统处理
BOOL CALLBACK MainDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
)
{
	//打开文件窗口参数结构体

	switch (uMsg)
	{//对话框初始化
	case WM_INITDIALOG:
	{
		//加载图标
		HICON hIcon = LoadIcon(hAppInstance, MAKEINTRESOURCE(IDI_ICON));
		//设置图标
		SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		//初始化
		InitProcessListControl(hDlg);
		InitModuleListControl(hDlg);

		return TRUE;
	}
	//相应点击按钮事件
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			//点关于按钮
		case IDC_BUTTON_ABOUT:
			DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), hDlg, AboutDialogProc);
			return TRUE;
			//点退出按钮
		case IDC_BUTTON_LOGOUT:
			//释放资源
			if (!fileBuffer)
			{
				free(fileBuffer);
			}
			EndDialog(hDlg, 0);
			return TRUE;
			//点查看按钮
		case IDC_BUTTON_PE:
		{
			OPENFILENAME openFile;
			TCHAR fileFilter[50] = TEXT("*.exe;*.dll;*.sys;*.scr;*.drv");

			//初始化
			memset(&openFile, 0, sizeof(OPENFILENAME));
			memset(fileName, 0, sizeof(fileName));

			//设置打开文件窗口参数
			openFile.lStructSize = sizeof(OPENFILENAME);
			openFile.hwndOwner = hDlg;
			openFile.lpstrFilter = fileFilter;
			openFile.lpstrTitle = TEXT("选择进程");
			openFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			openFile.lpstrFile = fileName;
			openFile.nMaxFile = 256;
			//获取选择的PE文件的路径
			GetOpenFileName(&openFile);
			//MessageBox(NULL, fileName, TEXT("进程名"), MB_OK);

			DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_PE), hDlg, PEDialogProc);
			DbgPrintf("pe %u\n", GetLastError());
			return TRUE;
		}
		default:
			break;
		}
	case WM_CLOSE:
		//释放资源
		if (!fileBuffer)
		{
			free(fileBuffer);
		}
		EndDialog(hDlg, 0);
		return TRUE;
	case WM_NOTIFY:
	{
		NM_LISTVIEW* pnmv = (NM_LISTVIEW FAR*)lParam;
		if (wParam == IDC_LIST_PROCESS && (pnmv->hdr).code == NM_CLICK)
		{
			EnumModule(hDlg, pnmv);
		}
		return TRUE;
	}
	default:
		break;
	}
	return FALSE;
}



/// @brief 初始化ModuleListControl
/// @param hDlg 对话框句柄
VOID InitModuleListControl(HWND hDlg)
{
	LV_COLUMN lv;
	HWND moduleList;
	TCHAR columnName[2][8] = { TEXT("模块名称"),TEXT("模块位置") };
	WORD columnWidth[2] = { 200, 300 };

	memset(&lv, 0, sizeof(LV_COLUMN));
	moduleList = GetDlgItem(hDlg, IDC_LIST_MODULE);
	//可以选中整行
	SendMessage(moduleList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	//循环设置列名
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	for (size_t i = 0; i < 2; i++)
	{
		lv.pszText = columnName[i];
		lv.cx = columnWidth[i];
		lv.iSubItem = i;
		ListView_InsertColumn(moduleList, i, &lv);
	}
	DbgPrintf("moduleinit %u\n", GetLastError());
}


/// @brief 初始化ProcessListControl
/// @param hDlg 对话框句柄
VOID InitProcessListControl(HWND hDlg)
{
	LV_COLUMN lv;
	HWND processList;
	TCHAR columnName[4][8] = { TEXT("进程"), TEXT("PID"), TEXT("镜像基址"), TEXT("镜像大小") };
	INT columnWidth[4] = { 195, 100, 100 ,100 };

	//初始化
	memset(&lv, 0, sizeof(LV_COLUMN));
	//获取 IDC_LIST_PROCESS 句柄
	processList = GetDlgItem(hDlg, IDC_LIST_PROCESS);
	//设置整行选中
	SendMessage(processList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	//循环增加列
	for (size_t i = 0; i < 4; i++)
	{
		lv.pszText = columnName[i];		//列标题
		lv.cx = columnWidth[i];			//列宽度
		lv.iSubItem = i;

		//SendMessage(processList, LVM_INSERTCOLUMN, i, (DWORD)&lv);
		ListView_InsertColumn(processList, i, &lv);				//和SendMessage效果一样
	}
	DbgPrintf("processinit %u\n", GetLastError());
	EnumProcess(processList);

}


/// @brief 向ListView中添加数据
/// @param processList ListView的句柄
BOOL EnumProcess(HWND processList)
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
				ListView_InsertItem(processList, &vItem);		//第一列使用InsertItem
			}
			else
			{
				ListView_SetItem(processList, &vItem);			//其他列使用SetItem
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
	HWND processList, moduleList;
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
	processList = GetDlgItem(hDlg, IDC_LIST_PROCESS);

	//获取点击IDC_LIST_PROCESS行的pid
	ListView_GetItemText(processList, pnmv->iItem, 1, pidBuffer, 20);

	//获取IDC_LIST_MODULE句柄
	moduleList = GetDlgItem(hDlg, IDC_LIST_MODULE);

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _ttoi(pidBuffer));
	//根据pid创建进程模块快照
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, _ttoi(pidBuffer));

	DbgPrintf("%u %d %d\n", GetLastError(), (DWORD)hModuleSnap, (DWORD)hProcess);

	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		ListView_DeleteAllItems(moduleList);
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
			ListView_InsertItem(moduleList, &lv);

			//插入第二列模块地址
			lv.pszText = moduleEntry.szExePath;
			lv.iItem = rowNumber;
			lv.iSubItem = 1;
			ListView_SetItem(moduleList, &lv);
		} while (Module32Next(hModuleSnap, &moduleEntry));
	}
	//关闭程序权限
	SetProcessPrivilege(SE_DEBUG_NAME, FALSE);
	CloseHandle(hModuleSnap);
	CloseHandle(hProcess);
	return TRUE;
}