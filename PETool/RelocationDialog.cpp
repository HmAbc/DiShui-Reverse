#include "RelocationDialog.h"

/// @brief �ض�λ�� �Ի�����Ϣ������
/// @param hDlg �Ի�����
/// @param uMsg ��Ϣ���
/// @param wParam ��Ϣ��һ������
/// @param lParam ��Ϣ�ڶ�������
/// @return ������Ϣ����󷵻�TRUE��δ������FALSE������ϵͳ����
BOOL CALLBACK RelocationDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		InitRelocationDialog(hDlg);
		return TRUE;

	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}


/// @brief ��ʼ���ض�λ��Ի����е�����
/// @param hDlg Ŀ¼�Ի���ľ��
/// @return �޴��󷵻�0���д��󷵻ش����룬800+
DWORD InitRelocationDialog(IN HWND hDlg)
{
	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS ntHeader = NULL;
	PIMAGE_FILE_HEADER peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;
	PIMAGE_DATA_DIRECTORY dataDirectory = NULL;
	PIMAGE_BASE_RELOCATION baseRelocation = NULL;
	HWND relocationEdit;
	TCHAR tempBuffer[80] = { 0 };
	//std::tstring* contentBuffer = new std::tstring();
	std::tstring contentBuffer;
	std::tstring tempStr;
	WORD index = 1;
	PWORD relocationAddr = NULL;
	DWORD relocationSize = 0;
	DWORD numberOfItem = 0;

	if (!fileBuffer)
	{
		return 800;
	}

	dosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	ntHeader = (PIMAGE_NT_HEADERS)((DWORD)fileBuffer + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);
	//��ȡ�༭����
	relocationEdit = GetDlgItem(hDlg, IDC_EDIT_RELOCATION);

	dataDirectory = optionalHeader->DataDirectory;
	//û���ض�λ��
	if (!dataDirectory[5].VirtualAddress)
	{
		contentBuffer += TEXT("\tû���ض�λ��\r\n");
		SendMessage(relocationEdit, EM_REPLACESEL, FALSE, (LPARAM)contentBuffer.c_str());
		return 801;
	}

	baseRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)RVA2FOA(fileBuffer, dataDirectory[5].VirtualAddress) + (DWORD)fileBuffer);
	contentBuffer += TEXT("RelocationTable:\r\n");

	while (baseRelocation->VirtualAddress)
	{
		relocationAddr = (PWORD)((DWORD)baseRelocation + 8);
		relocationSize = baseRelocation->SizeOfBlock;

		wsprintf(tempBuffer, TEXT("\r\n  %d. �ض�λ�飺\r\n\tVirtualAddress:\t%08X\r\n\tSizeOfBlock:\t%08X\r\n\r\n"), 
			index, baseRelocation->VirtualAddress, relocationSize);
		tempStr = tempBuffer;
		contentBuffer += tempStr;
		contentBuffer += TEXT("\t��ַ\t  ����\r\n\t���������������� ��������������\r\n");

		numberOfItem = (relocationSize - 8) / 2;
		for (DWORD i = 0; i < numberOfItem; i++)
		{
			wsprintf(tempBuffer, TEXT("\t%08X    %d\r\n"), relocationAddr[i] & 0xFFF + baseRelocation->VirtualAddress, (relocationAddr[i] >> 12) & 0xF);
			tempStr = tempBuffer;
			contentBuffer += tempStr;
		}

		index++;
		baseRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)baseRelocation + relocationSize);
	}
	
	SendMessage(relocationEdit, EM_REPLACESEL, FALSE, (LPARAM)contentBuffer.c_str());

	return 0;
}