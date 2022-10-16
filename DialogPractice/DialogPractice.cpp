#include <Windows.h>
#include "resource.h"

BOOL CALLBACK DialogProc(
	HWND hwndDlg,  // handle to dialog box			
	UINT uMsg,     // message			
	WPARAM wParam, // first message parameter			
	LPARAM lParam  // second message parameter			
)
{

	switch (uMsg)
	{
	case  WM_INITDIALOG:

		//MessageBox(NULL, TEXT("WM_INITDIALOG"), TEXT("INIT"), MB_OK);

		return TRUE;

	case  WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case   IDOK:

			MessageBox(NULL, TEXT("IDC_BUTTON_OK"), TEXT("OK"), MB_OK);

			return TRUE;

		case   IDCANCEL:

			MessageBox(NULL, TEXT("IDC_BUTTON_OUT"), TEXT("OUT"), MB_OK);

			EndDialog(hwndDlg, 0);

			return TRUE;
		}
		break;
	}

	return FALSE;
}


int APIENTRY WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd)
{
	DialogBox(hInstance, (LPCWCHAR)IDD_DIALOG_MAIN, NULL, DialogProc);

    return 0;
}