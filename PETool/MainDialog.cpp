#include "MainDialog.h"
#include "ProcessList.h"
#include "ModuleList.h"
#include "About.h"
#include "PEDialog.h"

//HINSTANCE hAppInstance;

/// @brief ����MAIN�Ի�����Ϣ����ص�����
/// @param hwnd ���ھ��
/// @param uMsg ��Ϣ����
/// @param wParam ��һ����Ϣ����
/// @param lParam �ڶ�����Ϣ����
/// @return ��������Ϣ����TRUE��δ�������Ϣ�ύ��ϵͳ����
BOOL CALLBACK MainDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
)
{
	//���ļ����ڲ����ṹ��
	OPENFILENAME openFile;

	switch (uMsg)
	{//�Ի����ʼ��
	case WM_INITDIALOG:
	{
		//����ͼ��
		HICON hIcon = LoadIcon(hAppInstance, MAKEINTRESOURCE(IDI_ICON));
		//����ͼ��
		SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
		//��ʼ��
		InitProcessListControl(hDlg);
		InitModuleListControl(hDlg);
		return TRUE;
	}
	//��Ӧ�����ť�¼�
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			//����ڰ�ť
		case IDC_BUTTON_ABOUT:
			DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_ABOUT), hDlg, AboutDialogProc);
			return TRUE;
			//���˳���ť
		case IDC_BUTTON_LOGOUT:
			EndDialog(hDlg, 0);
			return TRUE;
			//��鿴��ť
		case IDC_BUTTON_PE:
		{
			TCHAR fileFilter[50] = TEXT("*.exe;*.dll;*.sys;*.scr;*.drv");
			TCHAR fileName[256] = { 0 };
			//��ʼ��
			memset(&openFile, 0, sizeof(OPENFILENAME));

			//���ô��ļ����ڲ���
			openFile.lStructSize = sizeof(OPENFILENAME);
			openFile.hwndOwner = hDlg;
			openFile.lpstrFilter = fileFilter;
			openFile.lpstrTitle = TEXT("ѡ�����");
			openFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			openFile.lpstrFile = fileName;
			openFile.nMaxFile = 256;
			//��ȡѡ���PE�ļ���·��
			GetOpenFileName(&openFile);
			//MessageBox(NULL, fileName, TEXT("������"), MB_OK);
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
