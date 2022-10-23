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
	TCHAR pidBuffer[20] = {0};

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


