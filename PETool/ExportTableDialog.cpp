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
	TCHAR tempBuffer[60] = { 0 };
	TCHAR tempNameBuffer[20] = { 0 };
	std::tstring contentBuffer;
	std::tstring tempStr;
	BOOL flag = 0;
	PDWORD functionAddress;
	PWORD exportNumber;
	PDWORD nameAdderss;
	DWORD tempNameAddr = 0;

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
		contentBuffer += TEXT("\tû�е�����\r\n");
		SendMessage(exportEdit, EM_REPLACESEL, TRUE, (LPARAM)contentBuffer.c_str());
		return 501;
	}
	//������FOA
	exportDirectory = (PIMAGE_EXPORT_DIRECTORY)(RVA2FOA(fileBuffer, dataDirectory->VirtualAddress) + (DWORD)fileBuffer);
	//������ַFOA
	functionAddress = (PDWORD)(RVA2FOA(fileBuffer, exportDirectory->AddressOfFunctions) + (DWORD)fileBuffer);
	//�������FOA
	exportNumber = (PWORD)(RVA2FOA(fileBuffer, exportDirectory->AddressOfNameOrdinals) + (DWORD)fileBuffer);
	//��������FOA
	nameAdderss = (PDWORD)(RVA2FOA(fileBuffer, exportDirectory->AddressOfNames) + (DWORD)fileBuffer);

	contentBuffer += TEXT("ExportTable��\r\n");
	wsprintf(tempBuffer, TEXT("\tCharacteristics��\t%08X\r\n"), exportDirectory->Characteristics);
	tempStr = tempBuffer;
	contentBuffer += tempStr;
	wsprintf(tempBuffer, TEXT("\tTimeDataStamp��\t%08X\r\n"), exportDirectory->TimeDateStamp);
	tempStr = tempBuffer;
	contentBuffer += tempStr;
	wsprintf(tempBuffer, TEXT("\tMajorVersion��\t%04X\r\n"), exportDirectory->MajorVersion);
	tempStr = tempBuffer;
	contentBuffer += tempStr;
	wsprintf(tempBuffer, TEXT("\tMinorVersion��\t%04X\r\n"), exportDirectory->MinorVersion);
	tempStr = tempBuffer;
	contentBuffer += tempStr;
	wsprintf(tempBuffer, TEXT("\tName��\t\t%08X\r\n"), exportDirectory->Name);
	tempStr = tempBuffer;
	contentBuffer += tempStr;
	wsprintf(tempBuffer, TEXT("\tBase��\t\t%08X\r\n"), exportDirectory->Base);
	tempStr = tempBuffer;
	contentBuffer += tempStr;
	wsprintf(tempBuffer, TEXT("\tNumberOfFunctions��\t%08X\r\n"), exportDirectory->NumberOfFunctions);
	tempStr = tempBuffer;
	contentBuffer += tempStr;
	wsprintf(tempBuffer, TEXT("\tNumberOfNames��\t\t%08X\r\n"), exportDirectory->NumberOfNames);
	tempStr = tempBuffer;
	contentBuffer += tempStr;
	wsprintf(tempBuffer, TEXT("\tAddressOfFunctions��\t%08X\r\n"), exportDirectory->AddressOfFunctions);
	tempStr = tempBuffer;
	contentBuffer += tempStr;
	wsprintf(tempBuffer, TEXT("\tAddressOfNames��\t\t%08X\r\n"), exportDirectory->AddressOfNames);
	tempStr = tempBuffer;
	contentBuffer += tempStr;
	wsprintf(tempBuffer, TEXT("\tAddressOfNameOrdinals��\t%08X\r\n\r\n"), exportDirectory->AddressOfNameOrdinals);
	tempStr = tempBuffer;
	contentBuffer += tempStr;

	contentBuffer += TEXT("������ַ\t�������\t��������\r\n");

	for (DWORD i = 0; i < exportDirectory->NumberOfFunctions; i++)
	{
		for (DWORD j = 0; j < exportDirectory->NumberOfNames; j++)
		{
			if (i == exportNumber[j])
			{
				//���������֣���ANSCII���ַ���תΪUNICODE�ַ���
				tempNameAddr = RVA2FOA(fileBuffer, nameAdderss[j]) + (DWORD)fileBuffer;
				ANSIToUnicode((PCSTR)tempNameAddr, tempNameBuffer);

				wsprintf(tempBuffer, TEXT("%08X\t%04d\t\t%s\r\n"), functionAddress[i], exportNumber[j] + exportDirectory->Base, tempNameBuffer);
				tempStr = tempBuffer;
				contentBuffer += tempStr;
				flag = 1;
			}
		}
		if (!flag)
		{
			contentBuffer += std::to_tstring(functionAddress[i]) + TEXT("\r\n");
		}
		flag = 0;
	}

	SendMessage(exportEdit, EM_REPLACESEL, TRUE, (LPARAM)contentBuffer.c_str());

	return 0;
}