#include "ThreadTest.h"

HINSTANCE hAppInstance;
HWND edit1, edit2;

DWORD WINAPI ThreadProc1(LPVOID lpParameter);
DWORD WINAPI ThreadProc2(LPVOID lpParameter);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{

    hAppInstance = hInstance;
    DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, MainDialogProc);

    return 0;
}

BOOL CALLBACK MainDialogProc(
    HWND hDlg,		// handle of window
    UINT uMsg,		// message identifier
    WPARAM wParam,	// first message parameter
    LPARAM lParam	// second message parameter
)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        edit1 = GetDlgItem(hDlg, IDC_EDIT1);
        SetWindowText(edit1, TEXT("1000"));

        edit2 = GetDlgItem(hDlg, IDC_EDIT2);
        SetWindowText(edit2, TEXT("0"));
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON:
        {
            HANDLE thread1 = CreateThread(NULL, 0, ThreadProc1, NULL, 0, NULL);
            HANDLE thread2 = CreateThread(NULL, 0, ThreadProc2, NULL, 0, NULL);

            if (thread1 && thread2)
            {
                ::CloseHandle(thread1);
                ::CloseHandle(thread2);
            }
            
            return TRUE;
        }
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

DWORD WINAPI ThreadProc1(LPVOID lpParameter)
{
    //获取文本框数据
    TCHAR timeBuffer[10] = { 0 };
    DWORD time = 0;
    GetWindowText(edit1, timeBuffer, 10);
    swscanf_s(timeBuffer, TEXT("%d"), &time);

    while (time > 0)
    {
        memset(timeBuffer, 0, 10);
        Sleep(1000);
        wsprintf(timeBuffer, TEXT("%d"), --time);
        SetWindowText(edit1, timeBuffer);
    }

    return 0;
}

DWORD WINAPI ThreadProc2(LPVOID lpParameter)
{
    //获取文本框数据
    TCHAR timeBuffer[10] = { 0 };
    DWORD time = 0;
    GetWindowText(edit2, timeBuffer, 10);
    swscanf_s(timeBuffer, TEXT("%d"), &time);

    while (time < 1000)
    {
        memset(timeBuffer, 0, 10);
        Sleep(1000);
        wsprintf(timeBuffer, TEXT("%d"), ++time);
        SetWindowText(edit2, timeBuffer);
    }

    return 0;
}

