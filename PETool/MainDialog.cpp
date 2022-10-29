#include "MainDialog.h"
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
			//�ͷ���Դ
			if (!fileBuffer)
			{
				free(fileBuffer);
			}
			EndDialog(hDlg, 0);
			return TRUE;
			//��鿴��ť
		case IDC_BUTTON_PE:
		{
			OPENFILENAME openFile;
			TCHAR fileFilter[50] = TEXT("*.exe;*.dll;*.sys;*.scr;*.drv");

			//��ʼ��
			memset(&openFile, 0, sizeof(OPENFILENAME));
			memset(fileName, 0, sizeof(fileName));

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
			DbgPrintf("pe %u\n", GetLastError());
			return TRUE;
		}
		default:
			break;
		}
	case WM_CLOSE:
		//�ͷ���Դ
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



/// @brief ��ʼ��ModuleListControl
/// @param hDlg �Ի�����
VOID InitModuleListControl(HWND hDlg)
{
	LV_COLUMN lv;
	HWND moduleList;
	TCHAR columnName[2][8] = { TEXT("ģ������"),TEXT("ģ��λ��") };
	WORD columnWidth[2] = { 200, 300 };

	memset(&lv, 0, sizeof(LV_COLUMN));
	moduleList = GetDlgItem(hDlg, IDC_LIST_MODULE);
	//����ѡ������
	SendMessage(moduleList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	//ѭ����������
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


/// @brief ��ʼ��ProcessListControl
/// @param hDlg �Ի�����
VOID InitProcessListControl(HWND hDlg)
{
	LV_COLUMN lv;
	HWND processList;
	TCHAR columnName[4][8] = { TEXT("����"), TEXT("PID"), TEXT("�����ַ"), TEXT("�����С") };
	INT columnWidth[4] = { 195, 100, 100 ,100 };

	//��ʼ��
	memset(&lv, 0, sizeof(LV_COLUMN));
	//��ȡ IDC_LIST_PROCESS ���
	processList = GetDlgItem(hDlg, IDC_LIST_PROCESS);
	//��������ѡ��
	SendMessage(processList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	//ѭ��������
	for (size_t i = 0; i < 4; i++)
	{
		lv.pszText = columnName[i];		//�б���
		lv.cx = columnWidth[i];			//�п��
		lv.iSubItem = i;

		//SendMessage(processList, LVM_INSERTCOLUMN, i, (DWORD)&lv);
		ListView_InsertColumn(processList, i, &lv);				//��SendMessageЧ��һ��
	}
	DbgPrintf("processinit %u\n", GetLastError());
	EnumProcess(processList);

}


/// @brief ��ListView���������
/// @param processList ListView�ľ��
BOOL EnumProcess(HWND processList)
{
	LV_ITEM vItem;
	HANDLE hSnapshot, hModuleSnap;
	PROCESSENTRY32 processEntry;	//������Ž�����Ϣ�Ľṹ��
	MODULEENTRY32 moduleEntry;		//�������ģ����Ϣ�Ľṹ��
	BOOL isProcess = FALSE;
	INT rowNumber = 0;
	TCHAR* pid = new TCHAR[20]{ 0 };
	TCHAR* imageBase = new TCHAR[8]{ 0 };
	TCHAR* imageSize = new TCHAR[8]{ 0 };

	//��ʼ��
	memset(&vItem, 0, sizeof(LV_ITEM));
	vItem.mask = LVIF_TEXT;

	//�������̿���
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	//��ʼ���ṹ��
	processEntry.dwSize = sizeof(PROCESSENTRY32);
	moduleEntry.dwSize = sizeof(MODULEENTRY32);
	//�ӿ����ж�ȡ��һ��������Ϣ
	isProcess = Process32First(hSnapshot, &processEntry);
	while (isProcess)
	{
		//��ȡ��һ��ģ�飬�������������Ի��imageBase��
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
				ListView_InsertItem(processList, &vItem);		//��һ��ʹ��InsertItem
			}
			else
			{
				ListView_SetItem(processList, &vItem);			//������ʹ��SetItem
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

	//�����������ϵͳ���̵�Ȩ��
	isok = SetProcessPrivilege(SE_DEBUG_NAME, TRUE);

	//��ʼ��
	moduleEntry.dwSize = sizeof(MODULEENTRY32);
	lv.mask = TVIF_TEXT;


	//��ȡIDC_LIST_PROCESS���
	processList = GetDlgItem(hDlg, IDC_LIST_PROCESS);

	//��ȡ���IDC_LIST_PROCESS�е�pid
	ListView_GetItemText(processList, pnmv->iItem, 1, pidBuffer, 20);

	//��ȡIDC_LIST_MODULE���
	moduleList = GetDlgItem(hDlg, IDC_LIST_MODULE);

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _ttoi(pidBuffer));
	//����pid��������ģ�����
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
			//�����һ��ģ����
			lv.pszText = moduleEntry.szModule;
			lv.iItem = rowNumber;
			lv.iSubItem = 0;
			ListView_InsertItem(moduleList, &lv);

			//����ڶ���ģ���ַ
			lv.pszText = moduleEntry.szExePath;
			lv.iItem = rowNumber;
			lv.iSubItem = 1;
			ListView_SetItem(moduleList, &lv);
		} while (Module32Next(hModuleSnap, &moduleEntry));
	}
	//�رճ���Ȩ��
	SetProcessPrivilege(SE_DEBUG_NAME, FALSE);
	CloseHandle(hModuleSnap);
	CloseHandle(hProcess);
	return TRUE;
}