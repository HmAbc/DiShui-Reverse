#include "ModuleList.h"


/// @brief ��ʼ��ModuleListControl
/// @param hDlg �Ի�����
VOID InitModuleListControl(HWND hDlg)
{
	LV_COLUMN lv;
	HWND hListProcess;
	TCHAR columnName[2][8] = { TEXT("ģ������"),TEXT("ģ��λ��") };
	WORD columnWidth[2] = { 200, 300 };

	memset(&lv, 0, sizeof(LV_COLUMN));
	hListProcess = GetDlgItem(hDlg, IDC_LIST_MODULE);
	//����ѡ������
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	//ѭ����������
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	for (size_t i = 0; i < 2; i++)
	{
		lv.pszText = columnName[i];
		lv.cx = columnWidth[i];
		lv.iSubItem = i;
		ListView_InsertColumn(hListProcess, i, &lv);
	}
}
