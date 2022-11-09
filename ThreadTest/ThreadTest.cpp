/*
* 用临界区实现多线程读写控制
* 用互斥体完成相同功能
*/

#include "ThreadTest.h"
#include <iostream>

HINSTANCE hAppInstance;
HWND edit1, edit2, edit3, edit4;
CRITICAL_SECTION cs;
HANDLE mutex;

DWORD WINAPI ThreadProc0(LPVOID lpParameter);
DWORD WINAPI ThreadProc1(LPVOID lpParameter);
DWORD WINAPI ThreadProc2(LPVOID lpParameter);
DWORD WINAPI ThreadProc3(LPVOID lpParameter);

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
    {
        
        edit1 = GetDlgItem(hDlg, IDC_EDIT1);
        edit2 = GetDlgItem(hDlg, IDC_EDIT2);
        edit3 = GetDlgItem(hDlg, IDC_EDIT3);
        edit4 = GetDlgItem(hDlg, IDC_EDIT4);
        SetWindowText(edit1, TEXT("1000"));
        SetWindowText(edit2, TEXT("0"));
        SetWindowText(edit3, TEXT("0"));
        SetWindowText(edit4, TEXT("0"));
        return TRUE;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON:
        {
            HANDLE thread1 = ::CreateThread(NULL, 0, ThreadProc0, NULL, 0, NULL);
       

            if (thread1)
            {
                ::CloseHandle(thread1);
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

DWORD WINAPI ThreadProc0(LPVOID lpParameter)
{
    HANDLE handle[3];

    //初始化临界区
    InitializeCriticalSection(&cs);
    //创建互斥体
    mutex = CreateMutex(NULL, FALSE, NULL);

    SetWindowText(edit2, TEXT("0"));
    SetWindowText(edit3, TEXT("0"));
    SetWindowText(edit4, TEXT("0"));

    handle[0] = ::CreateThread(NULL, 0, ThreadProc1, NULL, 0, NULL);
    handle[1] = ::CreateThread(NULL, 0, ThreadProc2, NULL, 0, NULL);
    handle[2] = ::CreateThread(NULL, 0, ThreadProc3, NULL, 0, NULL);

    ::WaitForMultipleObjects(3, handle, TRUE, INFINITE);

    ::CloseHandle(handle[0]);
    ::CloseHandle(handle[1]);
    ::CloseHandle(handle[2]);
    ::CloseHandle(mutex);
    ::DeleteCriticalSection(&cs);
    return 0;
}


///使用临界区完成
//DWORD WINAPI ThreadProc1(LPVOID lpParameter)
//{
//    BOOL flag = 1;
//    //获取文本框数据
//    TCHAR editBuffer1[10] = { 0 };
//    TCHAR editBuffer2[10] = { 0 };
//    DWORD no1 = 0, no2 = 0;
//
//    while (flag)
//    {
//        EnterCriticalSection(&cs);
//        GetWindowText(edit1, editBuffer1, 10);
//        GetWindowText(edit2, editBuffer2, 10);
//        swscanf_s(editBuffer1, TEXT("%d"), &no1);
//        swscanf_s(editBuffer2, TEXT("%d"), &no2);
//        if (no1 <50)
//        {
//            flag = 0;
//        }
//        else
//        {
//            memset(editBuffer1, 0, 10);
//            memset(editBuffer2, 0, 10);
//            no1 -= 50;
//            no2 += 50;
//            wsprintf(editBuffer1, TEXT("%d"), no1);
//            wsprintf(editBuffer2, TEXT("%d"), no2);
//            SetWindowText(edit1, editBuffer1);
//            SetWindowText(edit2, editBuffer2);
//        }
//        LeaveCriticalSection(&cs);
//        Sleep(50);
//    }
//
//    return 0;
//}
//
//DWORD WINAPI ThreadProc2(LPVOID lpParameter)
//{
//    BOOL flag = 1;
//    //获取文本框数据
//    TCHAR editBuffer1[10] = { 0 };
//    TCHAR editBuffer2[10] = { 0 };
//    DWORD no1 = 0, no2 = 0;
//
//    while (flag)
//    {
//        EnterCriticalSection(&cs);
//        GetWindowText(edit1, editBuffer1, 10);
//        GetWindowText(edit3, editBuffer2, 10);
//        swscanf_s(editBuffer1, TEXT("%d"), &no1);
//        swscanf_s(editBuffer2, TEXT("%d"), &no2);
//        if (no1 < 50)
//        {
//            flag = 0;
//        }
//        else
//        {
//            memset(editBuffer1, 0, 10);
//            memset(editBuffer2, 0, 10);
//            no1 -= 50;
//            no2 += 50;
//            wsprintf(editBuffer1, TEXT("%d"), no1);
//            wsprintf(editBuffer2, TEXT("%d"), no2);
//            SetWindowText(edit1, editBuffer1);
//            SetWindowText(edit3, editBuffer2);
//        }
//        LeaveCriticalSection(&cs);
//        Sleep(40);
//    }
//
//    return 0;
//}
//
//DWORD WINAPI ThreadProc3(LPVOID lpParameter)
//{
//    BOOL flag = 1;
//    //获取文本框数据
//    TCHAR editBuffer1[10] = { 0 };
//    TCHAR editBuffer2[10] = { 0 };
//    DWORD no1 = 0, no2 = 0;
//
//    while (flag)
//    {
//        EnterCriticalSection(&cs);
//        GetWindowText(edit1, editBuffer1, 10);
//        GetWindowText(edit4, editBuffer2, 10);
//        swscanf_s(editBuffer1, TEXT("%d"), &no1);
//        swscanf_s(editBuffer2, TEXT("%d"), &no2);
//        if (no1 < 50)
//        {
//            flag = 0;
//        }
//        else
//        {
//            memset(editBuffer1, 0, 10);
//            memset(editBuffer2, 0, 10);
//            no1 -= 50;
//            no2 += 50;
//            wsprintf(editBuffer1, TEXT("%d"), no1);
//            wsprintf(editBuffer2, TEXT("%d"), no2);
//            SetWindowText(edit1, editBuffer1);
//            SetWindowText(edit4, editBuffer2);
//        }
//        LeaveCriticalSection(&cs);
//        Sleep(35);
//    }
//
//    return 0;
//}


///使用互斥体完成

DWORD WINAPI ThreadProc1(LPVOID lpParameter)
{
    //HANDLE mutex;
    BOOL flag = 1;
    //获取文本框数据
    TCHAR editBuffer1[10] = { 0 };
    TCHAR editBuffer2[10] = { 0 };
    DWORD no1 = 0, no2 = 0;

    while (flag)
    {
        //mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT("A"));
        WaitForSingleObject(mutex, INFINITE);
        GetWindowText(edit1, editBuffer1, 10);
        GetWindowText(edit2, editBuffer2, 10);
        swscanf_s(editBuffer1, TEXT("%d"), &no1);
        swscanf_s(editBuffer2, TEXT("%d"), &no2);
        
        if (no1 < 50)
        {
            flag = 0;
        }
        else
        {
            memset(editBuffer1, 0, 10);
            memset(editBuffer2, 0, 10);
            no1 -= 50;
            no2 += 50;
            wsprintf(editBuffer1, TEXT("%d"), no1);
            wsprintf(editBuffer2, TEXT("%d"), no2);
            SetWindowText(edit1, editBuffer1);
            SetWindowText(edit2, editBuffer2);
        }
        ReleaseMutex(mutex);
        Sleep(50);
    }

    return 0;
}

DWORD WINAPI ThreadProc2(LPVOID lpParameter)
{
    //HANDLE mutex;
    BOOL flag = 1;
    //获取文本框数据
    TCHAR editBuffer1[10] = { 0 };
    TCHAR editBuffer2[10] = { 0 };
    DWORD no1 = 0, no2 = 0;

    while (flag)
    {
        //mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT("A"));
        WaitForSingleObject(mutex, INFINITE);
        GetWindowText(edit1, editBuffer1, 10);
        GetWindowText(edit3, editBuffer2, 10);
        swscanf_s(editBuffer1, TEXT("%d"), &no1);
        swscanf_s(editBuffer2, TEXT("%d"), &no2);
        if (no1 < 50)
        {
            flag = 0;
        }
        else
        {
            memset(editBuffer1, 0, 10);
            memset(editBuffer2, 0, 10);
            no1 -= 50;
            no2 += 50;
            wsprintf(editBuffer1, TEXT("%d"), no1);
            wsprintf(editBuffer2, TEXT("%d"), no2);
            SetWindowText(edit1, editBuffer1);
            SetWindowText(edit3, editBuffer2);
        }
        ReleaseMutex(mutex);
        Sleep(40);
    }

    return 0;
}

DWORD WINAPI ThreadProc3(LPVOID lpParameter)
{
    //HANDLE mutex;
    BOOL flag = 1;
    //获取文本框数据
    TCHAR editBuffer1[10] = { 0 };
    TCHAR editBuffer2[10] = { 0 };
    DWORD no1 = 0, no2 = 0;

    while (flag)
    {
        //mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT("A"));
        WaitForSingleObject(mutex, INFINITE);
        GetWindowText(edit1, editBuffer1, 10);
        GetWindowText(edit4, editBuffer2, 10);
        swscanf_s(editBuffer1, TEXT("%d"), &no1);
        swscanf_s(editBuffer2, TEXT("%d"), &no2);
        if (no1 < 50)
        {
            flag = 0;
        }
        else
        {
            memset(editBuffer1, 0, 10);
            memset(editBuffer2, 0, 10);
            no1 -= 50;
            no2 += 50;
            wsprintf(editBuffer1, TEXT("%d"), no1);
            wsprintf(editBuffer2, TEXT("%d"), no2);
            SetWindowText(edit1, editBuffer1);
            SetWindowText(edit4, editBuffer2);
        }
        ReleaseMutex(mutex);
        Sleep(35);
    }

    return 0;
}



void __cdecl OutputDebugStringF(const char* format, ...)
{
    va_list vlArgs;
    char* strBuffer = (char*)GlobalAlloc(GPTR, 4096);

    va_start(vlArgs, format);
    _vsnprintf(strBuffer, 4096 - 1, format, vlArgs);
    va_end(vlArgs);
    strcat(strBuffer, "\n");
    OutputDebugStringA(strBuffer);
    GlobalFree(strBuffer);
    return;
}