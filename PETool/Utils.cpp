#define _CRT_SECURE_NO_WARNINGS
#pragma once
#include "Utils.h"
#include "resource.h"
#include "ProcessList.h"
#include "ModuleList.h"

extern HINSTANCE hAppInstance;

void __cdecl OutputDebugStringF(const char* format, ...)
{
	va_list vlArgs;
	char* strBuffer = (char*)GlobalAlloc(GPTR, 4096);

	va_start(vlArgs, format);
	_vsnprintf(strBuffer, 4096 - 1, format, vlArgs);
	va_end(vlArgs);
	strcat(strBuffer, "\n");
	OutputDebugStringA(strBuffer);
	GlobalFree(strBuffer);
	return;
}


//DIALOG 主窗口消息处理回调函数
BOOL CALLBACK MainDialogProc(
	HWND hDlg,
	// handle of window
	UINT uMsg,
	// message identifier
	WPARAM wParam,
	// first message parameter
	LPARAM lParam
	// second message parameter
)
{
	HICON hIcon;

	switch (uMsg)
	{//对话框初始化
	case WM_INITDIALOG:
		//加载图标

		hIcon = LoadIcon(hAppInstance, MAKEINTRESOURCE(IDI_ICON));
		//设置图标
		SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

		InitProcessListControl(hDlg);
		InitModuleListControl(hDlg);
		return TRUE;
	//相应点击按钮事件
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_ABOUT:

			return TRUE;
		case IDC_BUTTON_LOGOUT:
			EndDialog(hDlg, 0);
			return TRUE;
		case IDC_BUTTON_PE:

			return TRUE;
		default:
			break;
		}
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}


/// @brief 向ListView中添加数据
/// @param hListProcess ListView的句柄
BOOL EnumProcess(HWND hListProcess)
{
	LV_ITEM vItem;
	HANDLE hSnapshot;
	PROCESSENTRY32 processEntry;	//创建存放进程信息的结构体
	BOOL isProcess = FALSE;
	WORD rowNumber = 0;
	WCHAR pid[30] = { 0 };
	
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
	//从快照中读取第一个进程信息
	isProcess = Process32First(hSnapshot, &processEntry);
	while (isProcess)
	{

		for (size_t i = 0; i < 4; i++)
		{
			switch (i)
			{
			case 0:
				vItem.pszText = processEntry.szExeFile;
				break;
			case 1:
				wsprintf(pid, TEXT("%d"), processEntry.th32ProcessID);
				DbgPrintf("%s", pid);
				vItem.pszText = pid;
				break;
			case 2:
				vItem.pszText = TEXT("000000");
				break;
			case 3:
				vItem.pszText = TEXT("000000");
				break;
			default:
				break;
			}
			
			vItem.iItem = rowNumber;
			vItem.iSubItem = i;
			ListView_InsertItem(hListProcess, &vItem);
		}
		isProcess = Process32Next(hSnapshot, &processEntry);
		rowNumber++;
	}

	CloseHandle(hSnapshot);
	return TRUE;
}