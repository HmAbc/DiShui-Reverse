#include "PEDialog.h"
#include "DirectoryDialog.h"
#include "SectionDialog.h"

/// @brief ����PE��Ϣ�鿴�Ի�����Ϣ����ص�����
/// @param hwnd ���ھ��
/// @param uMsg ��Ϣ����
/// @param wParam ��һ����Ϣ����
/// @param lParam �ڶ�����Ϣ����
/// @return ��������Ϣ����TRUE��δ�������Ϣ�ύ��ϵͳ����
BOOL CALLBACK PEDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		InitPEImformation(fileName, hDlg);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			//�� ���� ��ťʱ���߼�
		case IDC_BUTTON_SECTION:
			DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_SECTION), hDlg, SectionDialogProc);
			return TRUE;
			//�� Ŀ¼ ��ťʱ���߼�
		case IDC_BUTTON_DIRECTORY:
			DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_DIRECTORY), hDlg, DirectoryDialogProc);
			return TRUE;
		default:
			break;
		}
		return TRUE;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

/// @brief ����PE�ļ�·������PE��Ϣ�鿴dialog����ʾ��Ϣ
/// @param filePath PE�ļ�·��
DWORD InitPEImformation(IN LPCTSTR filePath, IN HWND hDlg)
{
	TCHAR tempBuffer[40] = { 0 };
	DWORD fileSize = 0;
	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS ntHeader = NULL;
	PIMAGE_FILE_HEADER	peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;
	PIMAGE_SECTION_HEADER sectionHeader = NULL;

	fileSize = ReadPEFile(filePath, &fileBuffer);
	if (!fileBuffer)
	{
		return 200;
	}

	dosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	ntHeader = (PIMAGE_NT_HEADERS)((DWORD)fileBuffer + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);
	sectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)optionalHeader + peHeader->SizeOfOptionalHeader);

	//��ʼ�� PE�鿴 �Ի����е���Ϣ
	//��ڵ�
	wsprintf(tempBuffer, TEXT("%08X"), optionalHeader->AddressOfEntryPoint);
	SendDlgItemMessage(hDlg, IDC_EDIT_ENTRY, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//�����ַ
	wsprintf(tempBuffer, TEXT("%08X"), optionalHeader->ImageBase);
	SendDlgItemMessage(hDlg, IDC_EDIT_IMAGEBASE, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//�����С
	wsprintf(tempBuffer, TEXT("%08X"), optionalHeader->SizeOfImage);
	SendDlgItemMessage(hDlg, IDC_EDIT_IMAGESIZE, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//�����ַ
	wsprintf(tempBuffer, TEXT("%08X"), optionalHeader->BaseOfCode);
	SendDlgItemMessage(hDlg, IDC_EDIT_CODEBASE, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//���ݻ�ַ
	wsprintf(tempBuffer, TEXT("%08X"), optionalHeader->BaseOfData);
	SendDlgItemMessage(hDlg, IDC_EDIT_DATABASE, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//�ڴ����
	wsprintf(tempBuffer, TEXT("%08X"), optionalHeader->SectionAlignment);
	SendDlgItemMessage(hDlg, IDC_EDIT_MEMORYALIGN, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//�ļ�����
	wsprintf(tempBuffer, TEXT("%08X"), optionalHeader->FileAlignment);
	SendDlgItemMessage(hDlg, IDC_EDIT_FILEALIGN, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//��־��
	wsprintf(tempBuffer, TEXT("%04X"), optionalHeader->Magic);
	SendDlgItemMessage(hDlg, IDC_EDIT_SIGN, WM_SETTEXT, 0, (LPARAM)tempBuffer);

	//��ϵͳ
	wsprintf(tempBuffer, TEXT("%04X"), optionalHeader->Subsystem);
	SendDlgItemMessage(hDlg, IDC_EDIT_SUBSYS, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//������Ŀ
	wsprintf(tempBuffer, TEXT("%04X"), peHeader->NumberOfSections);
	SendDlgItemMessage(hDlg, IDC_EDIT_SECTIONNUMBER, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//ʱ���
	wsprintf(tempBuffer, TEXT("%08X"), peHeader->TimeDateStamp);
	SendDlgItemMessage(hDlg, IDC_EDIT_TIMESTAMP, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//PEͷ��С
	wsprintf(tempBuffer, TEXT("%08X"), optionalHeader->SizeOfHeaders);
	SendDlgItemMessage(hDlg, IDC_EDIT_HEADERSIZE, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//����ֵ
	wsprintf(tempBuffer, TEXT("%04X"), peHeader->Characteristics);
	SendDlgItemMessage(hDlg, IDC_EDIT_CHARACTERISTICS, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//�����
	wsprintf(tempBuffer, TEXT("%08X"), optionalHeader->CheckSum);
	SendDlgItemMessage(hDlg, IDC_EDIT_CHECKSUM, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//��ѡPEͷ
	wsprintf(tempBuffer, TEXT("%04X"), peHeader->SizeOfOptionalHeader);
	SendDlgItemMessage(hDlg, IDC_EDIT_OPTIONALHEADER, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//Ŀ¼����Ŀ
	wsprintf(tempBuffer, TEXT("%08X"), optionalHeader->NumberOfRvaAndSizes);
	SendDlgItemMessage(hDlg, IDC_EDIT_SECTIONITEMNUM, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	return 0;
}
