#include "SectionDialog.h"

/// @brief ���� �Ի�����Ϣ������
/// @param hDlg �Ի�����
/// @param uMsg ��Ϣ���
/// @param wParam ��Ϣ��һ������
/// @param lParam ��Ϣ�ڶ�������
/// @return ������Ϣ����󷵻�TRUE��δ������FALSE������ϵͳ����
BOOL CALLBACK SectionDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		InitSectionDialog(hDlg);
		return TRUE;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

/// @brief ��ʼ�� ���� �Ի����е�����
/// @param hDlg Ŀ¼�Ի���ľ��
/// @return �޴��󷵻�0���д��󷵻ش����룬400+
DWORD InitSectionDialog(IN HWND hDlg)
{
	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS ntHeader = NULL;
	PIMAGE_FILE_HEADER	peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;
	PIMAGE_SECTION_HEADER sectionHeader = NULL;
	LV_COLUMN lv;
	HWND sectionList;
	TCHAR columnName[5][10] = { TEXT("����"), TEXT("�ļ�ƫ��"), TEXT("�ļ���С"), TEXT("�ڴ�ƫ��"), TEXT("��������") };

	if (!fileBuffer)
	{
		DbgPrintf("no filebuffer");
		return 400;
	}

	dosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	ntHeader = (PIMAGE_NT_HEADERS)((DWORD)fileBuffer + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);
	sectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)peHeader + peHeader->SizeOfOptionalHeader);

	//��ʼ��
	lv.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	memset(&lv, 0, sizeof(LV_COLUMN));
	//DbgPrintf("InitSectionDialog %u %d\n", GetLastError(), sectionList);
	sectionList = GetDlgItem(hDlg, IDC_LIST_SECTION);
	//��������ѡ��Ч��
	SendMessage(sectionList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	//ѭ����������
	for (size_t i = 0; i < 5; i++)
	{
		lv.pszText = columnName[i];
		lv.cx = 75;
		lv.iSubItem = i;
		//SendMessage(sectionList, LVM_INSERTCOLUMN, i, (LPARAM)&lv);
		ListView_InsertColumn(sectionList, i, &lv);
	}
	DbgPrintf("InitSectionDialog %u %d\n", GetLastError(), sectionList);
	return 0;
}