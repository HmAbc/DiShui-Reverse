#include "PEDialog.h"
#include "DirectoryDialog.h"
#include "SectionDialog.h"

/// @brief 定义PE信息查看对话框消息处理回调函数
/// @param hwnd 窗口句柄
/// @param uMsg 消息类型
/// @param wParam 第一个消息参数
/// @param lParam 第二个消息参数
/// @return 处理完消息返回TRUE，未处理的消息会交给系统处理
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
			//按 区段 按钮时的逻辑
		case IDC_BUTTON_SECTION:
			DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_SECTION), hDlg, SectionDialogProc);
			return TRUE;
			//按 目录 按钮时的逻辑
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

/// @brief 输入PE文件路径，在PE信息查看dialog中显示信息
/// @param filePath PE文件路径
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

	//初始化 PE查看 对话框中的信息
	//入口点
	wsprintf(tempBuffer, TEXT("%08X"), optionalHeader->AddressOfEntryPoint);
	SendDlgItemMessage(hDlg, IDC_EDIT_ENTRY, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//镜像基址
	wsprintf(tempBuffer, TEXT("%08X"), optionalHeader->ImageBase);
	SendDlgItemMessage(hDlg, IDC_EDIT_IMAGEBASE, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//镜像大小
	wsprintf(tempBuffer, TEXT("%08X"), optionalHeader->SizeOfImage);
	SendDlgItemMessage(hDlg, IDC_EDIT_IMAGESIZE, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//代码基址
	wsprintf(tempBuffer, TEXT("%08X"), optionalHeader->BaseOfCode);
	SendDlgItemMessage(hDlg, IDC_EDIT_CODEBASE, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//数据基址
	wsprintf(tempBuffer, TEXT("%08X"), optionalHeader->BaseOfData);
	SendDlgItemMessage(hDlg, IDC_EDIT_DATABASE, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//内存对齐
	wsprintf(tempBuffer, TEXT("%08X"), optionalHeader->SectionAlignment);
	SendDlgItemMessage(hDlg, IDC_EDIT_MEMORYALIGN, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//文件对齐
	wsprintf(tempBuffer, TEXT("%08X"), optionalHeader->FileAlignment);
	SendDlgItemMessage(hDlg, IDC_EDIT_FILEALIGN, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//标志字
	wsprintf(tempBuffer, TEXT("%04X"), optionalHeader->Magic);
	SendDlgItemMessage(hDlg, IDC_EDIT_SIGN, WM_SETTEXT, 0, (LPARAM)tempBuffer);

	//子系统
	wsprintf(tempBuffer, TEXT("%04X"), optionalHeader->Subsystem);
	SendDlgItemMessage(hDlg, IDC_EDIT_SUBSYS, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//区段数目
	wsprintf(tempBuffer, TEXT("%04X"), peHeader->NumberOfSections);
	SendDlgItemMessage(hDlg, IDC_EDIT_SECTIONNUMBER, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//时间戳
	wsprintf(tempBuffer, TEXT("%08X"), peHeader->TimeDateStamp);
	SendDlgItemMessage(hDlg, IDC_EDIT_TIMESTAMP, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//PE头大小
	wsprintf(tempBuffer, TEXT("%08X"), optionalHeader->SizeOfHeaders);
	SendDlgItemMessage(hDlg, IDC_EDIT_HEADERSIZE, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//特征值
	wsprintf(tempBuffer, TEXT("%04X"), peHeader->Characteristics);
	SendDlgItemMessage(hDlg, IDC_EDIT_CHARACTERISTICS, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//检验和
	wsprintf(tempBuffer, TEXT("%08X"), optionalHeader->CheckSum);
	SendDlgItemMessage(hDlg, IDC_EDIT_CHECKSUM, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//可选PE头
	wsprintf(tempBuffer, TEXT("%04X"), peHeader->SizeOfOptionalHeader);
	SendDlgItemMessage(hDlg, IDC_EDIT_OPTIONALHEADER, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	//目录项数目
	wsprintf(tempBuffer, TEXT("%08X"), optionalHeader->NumberOfRvaAndSizes);
	SendDlgItemMessage(hDlg, IDC_EDIT_SECTIONITEMNUM, WM_SETTEXT, 0, (LPARAM)tempBuffer);
	return 0;
}
