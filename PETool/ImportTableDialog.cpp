#include "ImportTableDialog.h"

/// @brief ����� �Ի�����Ϣ������
/// @param hDlg �Ի�����
/// @param uMsg ��Ϣ���
/// @param wParam ��Ϣ��һ������
/// @param lParam ��Ϣ�ڶ�������
/// @return ������Ϣ����󷵻�TRUE��δ������FALSE������ϵͳ����
BOOL CALLBACK ImportDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		InitImportDialog(hDlg);
		return TRUE;

	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

/// @brief ��ʼ�������Ի����е�����
/// @param hDlg Ŀ¼�Ի���ľ��
/// @return �޴��󷵻�0���д��󷵻ش����룬600+
DWORD InitImportDialog(IN HWND hDlg)
{
	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS ntHeader = NULL;
	PIMAGE_FILE_HEADER	peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;
	PIMAGE_DATA_DIRECTORY dataDirectory = NULL;
	PIMAGE_IMPORT_DESCRIPTOR importDescriptor = NULL;
	PDWORD originalFirstThunk;
	PIMAGE_IMPORT_BY_NAME importByName;
	HWND importEdit;
	TCHAR tempBuffer[60] = { 0 };
	TCHAR tempNameBuffer[40] = { 0 };
	std::tstring contentBuffer;
	std::tstring tempStr;
	INT index = 1;
	DWORD tempNameAddr = 0;

	if (!fileBuffer)
	{
		return 600;
	}

	dosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	ntHeader = (PIMAGE_NT_HEADERS)((DWORD)fileBuffer + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);

	importEdit = GetDlgItem(hDlg, IDC_EDIT_IMPORTTABLE);

	dataDirectory = optionalHeader->DataDirectory;
	//û�е�����ʱ
	if (!dataDirectory[1].VirtualAddress)
	{
		contentBuffer += TEXT("\tû�е����\r\n");
		SendMessage(importEdit, EM_REPLACESEL, TRUE, (LPARAM)contentBuffer.c_str());
		return 601;
	}

	importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(RVA2FOA(fileBuffer, dataDirectory[1].VirtualAddress) + (DWORD)fileBuffer);
	contentBuffer += TEXT("ImportTable:\r\n");

	while (importDescriptor->OriginalFirstThunk)
	{
		contentBuffer += TEXT("\r\n-------------------------------------------\r\n");

		//����DLL����
		tempNameAddr = RVA2FOA(fileBuffer, importDescriptor->Name) + (DWORD)fileBuffer;
		ANSIToUnicode((PCSTR)tempNameAddr, tempNameBuffer);
		wsprintf(tempBuffer, TEXT("\t%d��%s\r\n"), index, tempNameBuffer);
		tempStr = tempBuffer;
		contentBuffer += tempStr;

		//���� OriginalFirstThunk
		originalFirstThunk = (PDWORD)(RVA2FOA(fileBuffer, importDescriptor->OriginalFirstThunk) + (DWORD)fileBuffer);
		contentBuffer += TEXT("\t\tOriginalFirstThunk:\r\n");

		while (*originalFirstThunk)
		{
			//����ŵ���
			if (*originalFirstThunk & IMAGE_ORDINAL_FLAG)
			{
				wsprintf(tempBuffer, TEXT("\t\t\t����ŵ��룺%d\r\n"), *originalFirstThunk & 0x7FFFFFFF);
			}
			else  //�����ֵ���
			{
				importByName = (PIMAGE_IMPORT_BY_NAME)(RVA2FOA(fileBuffer, *originalFirstThunk) + (DWORD)fileBuffer);
				//��������
				ANSIToUnicode((PCSTR)importByName->Name, tempNameBuffer);
				wsprintf(tempBuffer, TEXT("\t\t\t�����ֵ��룺HINT %X��%s\r\n"), importByName->Hint, tempNameBuffer);
				tempStr = tempBuffer;

				contentBuffer += tempStr;
			}
			originalFirstThunk++;
		}
		index++;
		importDescriptor++;
	}

	SendMessage(importEdit, EM_REPLACESEL, TRUE, (LPARAM)contentBuffer.c_str());

	return 0;
}
