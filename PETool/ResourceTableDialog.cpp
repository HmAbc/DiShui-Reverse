#include "ResourceTableDialog.h"

/// @brief ��Դ�� �Ի�����Ϣ������
/// @param hDlg �Ի�����
/// @param uMsg ��Ϣ���
/// @param wParam ��Ϣ��һ������
/// @param lParam ��Ϣ�ڶ�������
/// @return ������Ϣ����󷵻�TRUE��δ������FALSE������ϵͳ����
BOOL CALLBACK ResourceDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		InitResourceDialog(hDlg);
		return TRUE;

	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}


/// @brief ��ʼ����Դ��Ի����е�����
/// @param hDlg Ŀ¼�Ի���ľ��
/// @return �޴��󷵻�0���д��󷵻ش����룬700+
DWORD InitResourceDialog(IN HWND hDlg)
{
	PIMAGE_DOS_HEADER dosHeader = NULL;
	PIMAGE_NT_HEADERS ntHeader = NULL;
	PIMAGE_FILE_HEADER peHeader = NULL;
	PIMAGE_OPTIONAL_HEADER optionalHeader = NULL;
	PIMAGE_DATA_DIRECTORY dataDirectory = NULL;
	PIMAGE_RESOURCE_DIRECTORY resourceDir = NULL;
	PIMAGE_RESOURCE_DIRECTORY resourceDirSecond = NULL;
	PIMAGE_RESOURCE_DIRECTORY resourceDirThird = NULL;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY resourceDirEntry = NULL;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY resourceDirEntrySec = NULL;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY resourceDirEntryThird = NULL;
	PIMAGE_RESOURCE_DIR_STRING_U resourceDirStringU = NULL;
	HWND resourceEdit;
	TCHAR tempBuffer[40] = { 0 };
	TCHAR tempNameBuffer[20] = { 0 };
	std::tstring contentBuffer;
	std::tstring tempStr;
	
	TCHAR resourceType[17][8] = { 
		TEXT(""), TEXT("���"), TEXT("λͼ"), TEXT("ͼ��"), TEXT("�˵�"), TEXT("�Ի���"), 
		TEXT("�ַ���"), TEXT("����Ŀ¼"), TEXT("����"), TEXT("���ټ�"), TEXT("δ��ʽ����Դ"), TEXT("��Ϣ��"), 
		TEXT("�����"), TEXT(""), TEXT("ͼ����"), TEXT(""), TEXT("�汾��Ϣ") };

	if (!fileBuffer)
	{
		return 700;
	}

	dosHeader = (PIMAGE_DOS_HEADER)fileBuffer;
	ntHeader = (PIMAGE_NT_HEADERS)((DWORD)fileBuffer + dosHeader->e_lfanew);
	peHeader = (PIMAGE_FILE_HEADER)((DWORD)ntHeader + 4);
	optionalHeader = (PIMAGE_OPTIONAL_HEADER)((DWORD)peHeader + IMAGE_SIZEOF_FILE_HEADER);

	dataDirectory = optionalHeader->DataDirectory;
	DWORD t = RVA2FOA(fileBuffer, dataDirectory[2].VirtualAddress);
	resourceDir = (PIMAGE_RESOURCE_DIRECTORY)(t + (DWORD)fileBuffer);
	resourceDirEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)resourceDir + 16);

	contentBuffer += TEXT("ResourceTable:\r\n");

	//������Դ���һ�㣬��Դ���ͻ���ԴID
	for (WORD i = 0; i < resourceDir->NumberOfIdEntries + resourceDir->NumberOfNamedEntries; i++)
	{
		contentBuffer += TEXT("\r\n----------------------------------------------------------\r\n");
		if (resourceDirEntry[i].Name > 16)
		{
			wsprintf(tempBuffer, TEXT("��%d����Դ���ͣ�%s\r\n"), i + 1, TEXT("�Զ�������"));
			tempStr = tempBuffer;
			contentBuffer += tempStr;
		}
		else
		{
			wsprintf(tempBuffer, TEXT("��%d����Դ���ͣ�%s\r\n"), i + 1, resourceType[resourceDirEntry[i].Name]);
			tempStr = tempBuffer;
			contentBuffer += tempStr;
		}
		wsprintf(tempBuffer, TEXT("offset��%X\r\n"), resourceDirEntry[i].OffsetToDirectory);
		tempStr = tempBuffer;
		contentBuffer += tempStr;

		//������Դ��ڶ���
		//�ڶ���IMAGE_RESOURCE_DIRECTORY
		resourceDirSecond = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)resourceDir + resourceDirEntry[i].OffsetToDirectory);
		resourceDirEntrySec = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)resourceDirSecond + 16);
		for (WORD i = 0; i < resourceDirSecond->NumberOfIdEntries + resourceDirSecond->NumberOfNamedEntries; i++)
		{
			//�ж��Ƿ�������
			if (resourceDirEntrySec[i].NameIsString == 1)
			{
				resourceDirStringU = (PIMAGE_RESOURCE_DIR_STRING_U)((DWORD)resourceDir + resourceDirEntrySec[i].NameOffset);
				//��ȡ���ƣ����ַ���pResourceDirStringU->NameString���ַ�����ַ������Ҫ����ת��
				_tcsncpy_s(tempBuffer, 40, resourceDirStringU->NameString, resourceDirStringU->Length);
				//tempBuffer[resourceDirStringU->Length + 1] = '\0';
				tempStr = tempBuffer;
				contentBuffer += TEXT("\t��Դ���ƣ�") + tempStr + TEXT("\r\n");
			}
			else
			{
				wsprintf(tempBuffer, TEXT("\t��ԴID��%d\r\n"), resourceDirEntrySec[i].Id);
				tempStr = tempBuffer;
				contentBuffer += tempStr;
			}
			//������Դ������㣬ֻ��ӡRVA��SIZE
			resourceDirThird = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)resourceDir + resourceDirEntrySec[i].OffsetToDirectory);
			resourceDirEntryThird = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)resourceDirThird + 16);
			dataDirectory = (PIMAGE_DATA_DIRECTORY)((DWORD)resourceDir + resourceDirEntryThird->OffsetToData);

			wsprintf(tempBuffer, TEXT("\t\tRVA��%X,  SIZE��%X\r\n"), dataDirectory->VirtualAddress, dataDirectory->Size);
			tempStr = tempBuffer;
			contentBuffer += tempStr;
		}
	}

	resourceEdit = GetDlgItem(hDlg, IDC_EDIT_RESOURCETABLE);
	SendMessage(resourceEdit, EM_REPLACESEL, TRUE, (LPARAM)contentBuffer.c_str());

	return 0;
}
