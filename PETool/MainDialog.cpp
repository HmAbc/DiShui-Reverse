#include "MainDialog.h"
#include "ProcessList.h"
#include "ModuleList.h"
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
	OPENFILENAME openFile;

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
			EndDialog(hDlg, 0);
			return TRUE;
			//点查看按钮
		case IDC_BUTTON_PE:
		{
			TCHAR fileFilter[50] = TEXT("*.exe;*.dll;*.sys;*.scr;*.drv");
			TCHAR fileName[256] = { 0 };
			//初始化
			memset(&openFile, 0, sizeof(OPENFILENAME));

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
			DbgPrintf("%u\n", GetLastError());
			return TRUE;
		}
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
