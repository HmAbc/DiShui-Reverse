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
	DbgPrintf("%u\n", GetLastError());
	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS ntHeader = NULL;
	PIMAGE_FILE_HEADER	peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;
	PIMAGE_SECTION_HEADER sectionHeader = NULL;
	LV_COLUMN lv;
	LV_ITEM lvitem;
	HWND sectionList;
	TCHAR columnName[6][10] = { TEXT("����"), TEXT("�ļ�ƫ��"), TEXT("�ļ���С"), TEXT("�ڴ�ƫ��"), TEXT("�ڴ��С"), TEXT("��������") };
	//SECTIONINFO sectionInfo;
	CHAR tmpName[9] = { 0 };
	TCHAR sectionInfo[6][9];

	if (!fileBuffer)
	{
		DbgPrintf("no filebuffer");
		return 400;
	}

	dosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	ntHeader = (PIMAGE_NT_HEADERS)((DWORD)fileBuffer + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);
	sectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)optionalHeader + peHeader->SizeOfOptionalHeader);

	//��ʼ��
	memset(&lv, 0, sizeof(LV_COLUMN));
	lv.mask = LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;

	sectionList = GetDlgItem(hDlg, IDC_LIST_SECTION);
	//��������ѡ��Ч��
	SendMessage(sectionList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	//ѭ����������
	
	for (size_t i = 0; i < 6; i++)
	{
		lv.pszText = columnName[i];
		lv.cx = 100;
		lv.iSubItem = i;
		SendMessage(sectionList, LVM_INSERTCOLUMN, i, (LPARAM)&lv);
		//ListView_InsertColumn(sectionList, i, &lv);
	}

	//��ӽ���Ϣ
	memset(&lvitem, 0, sizeof(LV_ITEM));
	memset(&sectionInfo, 0, sizeof(sectionInfo));
	lvitem.mask = LVIF_TEXT;
	for (DWORD i = 0; i < peHeader->NumberOfSections; i++)
	{
		//����
		memcpy(tmpName, sectionHeader[i].Name, 8);
		ANSIToUnicode(tmpName, sectionInfo[0]);
		//�ļ�ƫ��
		wsprintf(sectionInfo[1], TEXT("%08X"), sectionHeader[i].PointerToRawData);
		//�ļ���С
		wsprintf(sectionInfo[2], TEXT("%08X"), sectionHeader[i].SizeOfRawData);
		//�ڴ�ƫ��
		wsprintf(sectionInfo[3], TEXT("%08X"), sectionHeader[i].VirtualAddress);
		//�ڴ��С
		wsprintf(sectionInfo[4], TEXT("%08X"), sectionHeader[i].Misc.VirtualSize);
		//��������
		wsprintf(sectionInfo[5], TEXT("%08X"), sectionHeader[i].Characteristics);
		for (size_t j = 0; j < 6; j++)
		{
			lvitem.pszText = sectionInfo[j];
			lvitem.iItem = i;
			lvitem.iSubItem = j;
			
			if (j == 0)
			{
				SendMessage(sectionList, LVM_INSERTITEM, 0, (LPARAM)&lvitem);
			}
			else
			{
				SendMessage(sectionList, LVM_SETITEM, 0, (LPARAM)&lvitem);
			}
		}			
	}

	return 0;
}
