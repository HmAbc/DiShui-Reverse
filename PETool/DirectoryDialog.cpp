#include "DirectoryDialog.h"

/// @brief Ŀ¼ �Ի�����Ϣ������
/// @param hDlg �Ի�����
/// @param uMsg ��Ϣ���
/// @param wParam ��Ϣ��һ������
/// @param lParam ��Ϣ�ڶ�������
/// @return ������Ϣ����󷵻�TRUE��δ������FALSE������ϵͳ����
BOOL CALLBACK DirectoryDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:;
		InitDirectoryDialog(hDlg);
		DbgPrintf("InitDirectoryDialog %u\n", GetLastError());
		return TRUE;

	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}


/// @brief ��ʼ��Ŀ¼�Ի����е�����
/// @param hDlg Ŀ¼�Ի���ľ��
/// @return �޴��󷵻�0���д��󷵻ش�����300+
DWORD InitDirectoryDialog(IN HWND hDlg)
{
	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS ntHeader = NULL;
	PIMAGE_FILE_HEADER	peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;
	PIMAGE_DATA_DIRECTORY dataDirectory = NULL;

	INT rva[16] = { IDC_EDIT_RVA1, IDC_EDIT_RVA2, IDC_EDIT_RVA3, IDC_EDIT_RVA4, IDC_EDIT_RVA5, 
		IDC_EDIT_RVA6, IDC_EDIT_RVA7, IDC_EDIT_RVA8, IDC_EDIT_RVA9, IDC_EDIT_RVA10, IDC_EDIT_RVA11, 
		IDC_EDIT_RVA12, IDC_EDIT_RVA13, IDC_EDIT_RVA14, IDC_EDIT_RVA15, IDC_EDIT_RVA16 };
	INT size[16] = { IDC_EDIT_SIZE1, IDC_EDIT_SIZE2, IDC_EDIT_SIZE3, IDC_EDIT_SIZE4, IDC_EDIT_SIZE5,
		IDC_EDIT_SIZE6, IDC_EDIT_SIZE7, IDC_EDIT_SIZE8, IDC_EDIT_SIZE9, IDC_EDIT_SIZE10, IDC_EDIT_SIZE11,
		IDC_EDIT_SIZE12, IDC_EDIT_SIZE13, IDC_EDIT_SIZE14, IDC_EDIT_SIZE15, IDC_EDIT_SIZE16 };
	TCHAR temp[20] = { 0 };
	//TCHAR rvaTemp[20] = TEXT("IDC_EDIT_RVA");
	//TCHAR sizeTemp[20] = TEXT("IDC_EDIT_SIZE");
	//TCHAR index[3] = { 0 };

	//fileSize = ReadPEFile(filePath, &fileBuffer);
	if (!fileBuffer)
	{
		return 300;
	}

	dosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	ntHeader = (PIMAGE_NT_HEADERS)((DWORD)fileBuffer + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);

	dataDirectory = optionalHeader->DataDirectory;

	for (DWORD i = 0; i < 16; i++)
	{
		//����rva
		wsprintf(temp, TEXT("%08X"), dataDirectory[i].VirtualAddress);
		SendDlgItemMessage(hDlg, rva[i], WM_SETTEXT, 0, (LPARAM)temp);

		//����size
		wsprintf(temp, TEXT("%08X"), dataDirectory[i].Size);
		SendDlgItemMessage(hDlg, size[i], WM_SETTEXT, 0, (LPARAM)temp);

	}

	return 0;
}