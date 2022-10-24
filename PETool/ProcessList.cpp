#include "ProcessList.h"
#include <psapi.h>

/// @brief ��ʼ��ProcessListControl
/// @param hDlg �Ի�����
VOID InitProcessListControl(HWND hDlg)
{
	LV_COLUMN lv;
	HWND hListProcess;
	TCHAR columnName[4][8] = { TEXT("����"), TEXT("PID"), TEXT("�����ַ"), TEXT("�����С") };
	WORD columnWidth[4] = { 195, 100, 100 ,100 };

	//��ʼ��
	memset(&lv, 0, sizeof(LV_COLUMN));
	//��ȡ IDC_LIST_PROCESS ���
	hListProcess = GetDlgItem(hDlg, IDC_LIST_PROCESS);
	//��������ѡ��
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	//ѭ��������
	for (size_t i = 0; i < 4; i++)
	{
		lv.pszText = columnName[i];		//�б���
		lv.cx = columnWidth[i];			//�п��
		lv.iSubItem = i;

		//SendMessage(hListProcess, LVM_INSERTCOLUMN, i, (DWORD)&lv);
		ListView_InsertColumn(hListProcess, i, &lv);				//��SendMessageЧ��һ��
	}
	EnumProcess(hListProcess);

}


/// @brief ��ListView���������
/// @param hListProcess ListView�ľ��
BOOL EnumProcess(HWND hListProcess)
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
				ListView_InsertItem(hListProcess, &vItem);		//��һ��ʹ��InsertItem
			}
			else
			{
				ListView_SetItem(hListProcess, &vItem);			//������ʹ��SetItem
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

	//�����������ϵͳ���̵�Ȩ��
	isok = SetProcessPrivilege(SE_DEBUG_NAME, TRUE);

	//��ʼ��
	moduleEntry.dwSize = sizeof(MODULEENTRY32);
	lv.mask = TVIF_TEXT;


	//��ȡIDC_LIST_PROCESS���
	hListProcess = GetDlgItem(hDlg, IDC_LIST_PROCESS);

	//��ȡ���IDC_LIST_PROCESS�е�pid
	ListView_GetItemText(hListProcess, pnmv->iItem, 1, pidBuffer, 20);

	//��ȡIDC_LIST_MODULE���
	hListModule = GetDlgItem(hDlg, IDC_LIST_MODULE);

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _ttoi(pidBuffer));
	//����pid��������ģ�����
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
			//�����һ��ģ����
			lv.pszText = moduleEntry.szModule;
			lv.iItem = rowNumber;
			lv.iSubItem = 0;
			ListView_InsertItem(hListModule, &lv);

			//����ڶ���ģ���ַ
			lv.pszText = moduleEntry.szExePath;
			lv.iItem = rowNumber;
			lv.iSubItem = 1;
			ListView_SetItem(hListModule, &lv);
		} while (Module32Next(hModuleSnap, &moduleEntry));
	}
	//�رճ���Ȩ��
	SetProcessPrivilege(SE_DEBUG_NAME, FALSE);
	CloseHandle(hModuleSnap);
	CloseHandle(hProcess);
	return TRUE;
}