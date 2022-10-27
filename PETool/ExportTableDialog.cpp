#include "ExportTableDialog.h"

/// @brief ������ �Ի�����Ϣ������
/// @param hDlg �Ի�����
/// @param uMsg ��Ϣ���
/// @param wParam ��Ϣ��һ������
/// @param lParam ��Ϣ�ڶ�������
/// @return ������Ϣ����󷵻�TRUE��δ������FALSE������ϵͳ����
BOOL CALLBACK ExportDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		InitExportTableDialog(hDlg);
		return TRUE;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

/// @brief ��ӡ����������
/// @param hDlg �Ի�����
/// @return �޴��󷵻�0���д��󷵻ش�����500+
DWORD InitExportTableDialog(IN HWND hDlg)
{
	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS ntHeader = NULL;
	PIMAGE_FILE_HEADER	peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;
	PIMAGE_DATA_DIRECTORY dataDirectory = NULL;
	PIMAGE_EXPORT_DIRECTORY exportDirectory = NULL;
	HWND exportEdit;
	TCHAR tmpBuffer[20] = { 0 };

	if (!fileBuffer)
	{
		return 500;
	}

	dosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	ntHeader = (PIMAGE_NT_HEADERS)((DWORD)fileBuffer + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);

	exportEdit = GetDlgItem(hDlg, IDC_EDIT_EXPORTTABLE);

	dataDirectory = optionalHeader->DataDirectory;
	//û�е�����ʱ
	if (!dataDirectory->VirtualAddress)
	{
		SendMessage(exportEdit, EM_REPLACESEL, TRUE, (LPARAM)TEXT("û�е�����"));
		return 501;
	}

	exportDirectory = (PIMAGE_EXPORT_DIRECTORY)(RVA2FOA(fileBuffer, dataDirectory->VirtualAddress) + (DWORD)fileBuffer);
	SendMessage(exportEdit, EM_REPLACESEL, TRUE, (LPARAM)TEXT("û�е�����"));
	

	return 0;
}