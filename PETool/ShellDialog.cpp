#include "ShellDialog.h"

TCHAR nameBuffer[256] = { 0 };
HWND editShell, editProcess;
HANDLE event;

DWORD WINAPI test(LPVOID lpParameter);

/// @brief 加壳 对话框消息处理函数
/// @param hDlg 对话框句柄
/// @param uMsg 消息编号
/// @param wParam 消息第一个参数
/// @param lParam 消息第二个参数
/// @return 拦截消息处理后返回TRUE，未处理返回FALSE，交给系统处理
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
		//获取两个编辑框句柄
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

			//初始化
			memset(&openFile, 0, sizeof(OPENFILENAME));
			memset(nameBuffer, 0, sizeof(nameBuffer));

			//设置打开文件窗口参数
			openFile.lStructSize = sizeof(OPENFILENAME);
			openFile.hwndOwner = hDlg;
			openFile.lpstrFilter = fileFilter;
			openFile.lpstrTitle = TEXT("选择壳程序");
			openFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			openFile.lpstrFile = nameBuffer;
			openFile.nMaxFile = 256;
			//获取选择的PE文件的路径
			GetOpenFileName(&openFile);
			//MessageBox(NULL, fileName, TEXT("进程名"), MB_OK);
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

			//初始化
			memset(&openFile, 0, sizeof(OPENFILENAME));
			memset(nameBuffer, 0, sizeof(nameBuffer));

			//设置打开文件窗口参数
			openFile.lStructSize = sizeof(OPENFILENAME);
			openFile.hwndOwner = hDlg;
			openFile.lpstrFilter = fileFilter;
			openFile.lpstrTitle = TEXT("选择需加壳程序");
			openFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
			openFile.lpstrFile = nameBuffer;
			openFile.nMaxFile = 256;
			//获取选择的PE文件的路径
			GetOpenFileName(&openFile);
			//MessageBox(NULL, fileName, TEXT("进程名"), MB_OK);
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

			MessageBox(hDlg, nameBuffer, TEXT("文件保存位置"), MB_OK);
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

/// @brief 加壳线程
/// @param lpParameter 创建线程时传递的参数 
/// @return 状态码
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
	//读取两个文件
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
	//移动壳程序pe头，如果移动失败，释放所有资源
	if (!MoveHeader(shellBuffer))
	{
		free(shellBuffer);
		free(processBuffer);
		return 0;
	}
	//在壳程序后添加一个节，用来存放需要保护的程序数据，如果添加失败，释放所有资源
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

	//把要保护的程序复制到壳程序新增的节中，先获取目的地址
	dstAddr = (LPVOID)((DWORD)newBuffer + sectionHeader[sectionNumber - 1].PointerToRawData);
	//异或加密文件
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

	//解析待保护程序路径
	_tsplitpath_s(processName, drive, dir, fname, ext);
	wsprintf(nameBuffer, TEXT("%s%s%s_protected%s"), drive, dir, fname, ext);

	//保存文件
	MemoryToFile(newBuffer, totalSize, nameBuffer);
	//释放资源
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