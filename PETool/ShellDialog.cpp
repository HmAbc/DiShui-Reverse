#include "ShellDialog.h"

TCHAR nameBuffer[256] = { 0 };
HWND editShell, editProcess;
HANDLE event;

DWORD WINAPI test(LPVOID lpParameter);

/// @brief �ӿ� �Ի�����Ϣ������
/// @param hDlg �Ի�����
/// @param uMsg ��Ϣ���
/// @param wParam ��Ϣ��һ������
/// @param lParam ��Ϣ�ڶ�������
/// @return ������Ϣ����󷵻�TRUE��δ������FALSE������ϵͳ����
BOOL CALLBACK ShellDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		//��ȡ�����༭����
		editShell = GetDlgItem(hDlg, IDC_EDIT_SHELL);
		editProcess = GetDlgItem(hDlg, IDC_EDIT_PROCESS);


		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_CHOOSESHELL:
		{
			OPENFILENAME openFile;
			TCHAR fileFilter[20] = TEXT("*.exe\0*.exe\0");

			//��ʼ��
			memset(&openFile, 0, sizeof(OPENFILENAME));
			memset(nameBuffer, 0, sizeof(nameBuffer));

			//���ô��ļ����ڲ���
			openFile.lStructSize = sizeof(OPENFILENAME);
			openFile.hwndOwner = hDlg;
			openFile.lpstrFilter = fileFilter;
			openFile.lpstrTitle = TEXT("ѡ��ǳ���");
			openFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			openFile.lpstrFile = nameBuffer;
			openFile.nMaxFile = 256;
			//��ȡѡ���PE�ļ���·��
			GetOpenFileName(&openFile);
			//MessageBox(NULL, fileName, TEXT("������"), MB_OK);
			if (lstrcmp(nameBuffer, TEXT("")))
			{
				SetWindowText(editShell, nameBuffer);
			}
			return TRUE;
		}
		case IDC_BUTTON_CHOOSEPROC:
		{
			OPENFILENAME openFile;
			TCHAR fileFilter[30] = TEXT("*.exe\0*.exe\0");

			//��ʼ��
			memset(&openFile, 0, sizeof(OPENFILENAME));
			memset(nameBuffer, 0, sizeof(nameBuffer));

			//���ô��ļ����ڲ���
			openFile.lStructSize = sizeof(OPENFILENAME);
			openFile.hwndOwner = hDlg;
			openFile.lpstrFilter = fileFilter;
			openFile.lpstrTitle = TEXT("ѡ����ӿǳ���");
			openFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			openFile.lpstrFile = nameBuffer;
			openFile.nMaxFile = 256;
			//��ȡѡ���PE�ļ���·��
			GetOpenFileName(&openFile);
			//MessageBox(NULL, fileName, TEXT("������"), MB_OK);
			if (lstrcmp(nameBuffer, TEXT("")))
			{
				SetWindowText(editProcess, nameBuffer);
			}
			return TRUE;
		}
		case IDC_BUTTON_START:
		{
			/*HANDLE threadHandle = CreateThread(NULL, 0, test, NULL, 0, NULL);
			
			WaitForSingleObject(threadHandle, INFINITE);*/
			AddShell((void*)1);

			MessageBox(hDlg, nameBuffer, TEXT("�ļ�����λ��"), MB_OK);
			return TRUE;
		}
		case IDC_BUTTON_RETURN:
			EndDialog(hDlg, 0);
			return TRUE;
		default:
			break;
		}
		return FALSE;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

/// @brief �ӿ��߳�
/// @param lpParameter �����߳�ʱ���ݵĲ��� 
/// @return ״̬��
DWORD WINAPI AddShell(LPVOID lpParameter)
{
	LPVOID shellBuffer = NULL;
	LPVOID processBuffer = NULL;
	LPVOID newBuffer = NULL;
	PDWORD encryptBuffer = NULL;
	LPVOID dstAddr = NULL;
	TCHAR shellName[256] = { 0 };
	TCHAR processName[256] = { 0 };
	DWORD shellSize = 0, processSize = 0, totalSize = 0;
	DWORD virtualSize = 0;
	DWORD sectionNumber = 0;
	CHAR newSectionName[8] = "PYF";
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];


	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS ntHeader = NULL;
	PIMAGE_FILE_HEADER peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;
	PIMAGE_SECTION_HEADER sectionHeader = NULL;

	GetWindowText(editShell, shellName, 256);
	GetWindowText(editProcess, processName, 256);
	//��ȡ�����ļ�
	shellSize = ReadPEFile(shellName, &shellBuffer);
	if (!shellBuffer)
	{
		return 0;
	}
	processSize = ReadPEFile(processName, &processBuffer);
	if (!processBuffer)
	{
		free(shellBuffer);
		return 0;
	}
	//�ƶ��ǳ���peͷ������ƶ�ʧ�ܣ��ͷ�������Դ
	if (!MoveHeader(shellBuffer))
	{
		free(shellBuffer);
		free(processBuffer);
		return 0;
	}
	//�ڿǳ�������һ���ڣ����������Ҫ�����ĳ������ݣ�������ʧ�ܣ��ͷ�������Դ
	totalSize = AddNewSection(shellBuffer, &newBuffer, shellSize, processSize, newSectionName);
	if (!totalSize)
	{
		free(shellBuffer);
		free(processBuffer);
		return 0;
	}

	dosHeader = (PIMAGE_DOS_HEADER)newBuffer;
	ntHeader = (PIMAGE_NT_HEADERS)((DWORD)dosHeader + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);
	sectionHeader = (PIMAGE_SECTION_HEADER)((DWORD)optionalHeader + peHeader->SizeOfOptionalHeader);

	sectionNumber = peHeader->NumberOfSections;

	//��Ҫ�����ĳ����Ƶ��ǳ��������Ľ��У��Ȼ�ȡĿ�ĵ�ַ
	dstAddr = (LPVOID)((DWORD)newBuffer + sectionHeader[sectionNumber - 1].PointerToRawData);
	//�������ļ�
	encryptBuffer = (PDWORD)malloc(processSize);
	if (!encryptBuffer)
	{
		free(shellBuffer);
		free(processBuffer);
		free(newBuffer);
		return 0;
	}
	for (int i = 0; i < processSize / 4; i++)
	{
		encryptBuffer[i] = ((PDWORD)processBuffer)[i] ^ KEY;
	}
	memcpy(dstAddr, encryptBuffer, processSize);

	//��������������·��
	_tsplitpath_s(processName, drive, dir, fname, ext);
	wsprintf(nameBuffer, TEXT("%s%s%s_protected%s"), drive, dir, fname, ext);

	//�����ļ�
	MemoryToFile(newBuffer, totalSize, nameBuffer);
	//�ͷ���Դ
	free(shellBuffer);
	free(processBuffer);
	free(newBuffer);
	free(encryptBuffer);

	return 1;
}

DWORD WINAPI test(LPVOID lpParameter)
{
	TCHAR processName[256] = { 0 };
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];

	GetWindowText(editProcess, processName, 256);

	_tsplitpath_s(processName, drive, dir, fname, ext);

	wsprintf(nameBuffer, TEXT("%s%s%s_protected%s"), drive, dir, fname, ext);
	
	return 0;

}