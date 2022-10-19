#include "ProcessList.h"

/// @brief ��ʼ��ProcessListControl
/// @param hDlg �Ի�����
VOID InitProcessListControl(HWND hDlg)
{
	LV_COLUMN lv;
	HWND hListProcess;
	TCHAR columnName[4][8] = { TEXT("����"), TEXT("PID"), TEXT("�����ַ"), TEXT("�����С") };
	WORD columnWidth[4] = { 200, 100, 100 ,100 };

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
