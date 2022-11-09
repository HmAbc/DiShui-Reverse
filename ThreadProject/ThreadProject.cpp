#include <Windows.h>
#include "resource.h"

HINSTANCE appInstance;
HWND editInput, editBuffer1, editBuffer2, editA, editB, editC, editD;
HWND hwndBuffer[2], handleEdit[4];
//�ź���
HANDLE semaphoreEmpty, semaphoreFull;
//�ٽ���
CRITICAL_SECTION cs;
HANDLE handleThread[4];

BOOL CALLBACK MainDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD WINAPI StartThread(LPVOID lpParameter);
DWORD WINAPI ServerThread(LPVOID lpParameter);
DWORD WINAPI ClientThread(LPVOID lpParameter);

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    appInstance = hInstance;

    DialogBox(appInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, MainDialogProc);


    return 0;
}


BOOL CALLBACK MainDialogProc(
    HWND hDlg,       // handle to dialog box
    UINT uMsg,          // message
    WPARAM wParam,      // first message parameter
    LPARAM lParam       // second message parameter
)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        editInput = GetDlgItem(hDlg, IDC_EDIT_INPUT);
        editBuffer1 = GetDlgItem(hDlg, IDC_EDIT_BUFFER1);
        editBuffer2 = GetDlgItem(hDlg, IDC_EDIT_BUFFER2);
        hwndBuffer[0] = editBuffer1;
        hwndBuffer[1] = editBuffer2;

        editA = GetDlgItem(hDlg, IDC_EDIT_A);
        editB = GetDlgItem(hDlg, IDC_EDIT_B);
        editC = GetDlgItem(hDlg, IDC_EDIT_C);
        editD = GetDlgItem(hDlg, IDC_EDIT_D);

        handleEdit[0] = editA;
        handleEdit[1] = editB;
        handleEdit[2] = editC;
        handleEdit[3] = editD;

        SetWindowText(editInput, TEXT("0"));
        SetWindowText(editBuffer1, TEXT("0"));
        SetWindowText(editBuffer2, TEXT("0"));
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_START:
        {
            HANDLE startHandle = ::CreateThread(NULL, 0, StartThread, NULL, 0, NULL);
            if (startHandle)
            {
                ::CloseHandle(startHandle);
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

DWORD WINAPI StartThread(LPVOID lpParameter)
{
    //�����ź����������������̺߳��������߳�ͬ��
    semaphoreEmpty = CreateSemaphore(NULL, 2, 2, NULL);
    semaphoreFull = CreateSemaphore(NULL, 0, 2, NULL);
    //��ʼ���ٽ��������ڻ���������
    InitializeCriticalSection(&cs);

    //�����������߳�
    HANDLE serverThread = ::CreateThread(NULL, 0, ServerThread, NULL, 0, NULL);
    CloseHandle(serverThread);

    //�����ĸ��������߳�
    for (size_t i = 0; i < 4; i++)
    {
        //����һ������ֵ��������ʾ���ֲ�ͬ���߳�
        handleThread[i] = ::CreateThread(NULL, 0, ClientThread, (LPVOID)i, 0, NULL);
    }

    ::WaitForMultipleObjects(4, handleThread, TRUE, INFINITE);
    for (size_t i = 0; i < 4; i++)
    {
        ::CloseHandle(handleThread[i]);
    }
    ::CloseHandle(semaphoreEmpty);
    ::CloseHandle(semaphoreFull);
    ::DeleteCriticalSection(&cs);

    return 0;
}


DWORD WINAPI ServerThread(LPVOID lpParameter)
{
    DWORD len = 0;
    DWORD ret1 = 0, ret2 = 0;
    TCHAR resourceBuffer[100] = { 0 };
    TCHAR tempBuffer[2] = { 0 };

    //��ȡ��Դ�ı�
    GetWindowText(editInput, resourceBuffer, 100);
    //��ȡ�����ı�����
    len = lstrlen(resourceBuffer);
    if (len == 0)
    {
        return -1;
    }
    for (size_t i = 0; i < len; i++)
    {
        WaitForSingleObject(semaphoreEmpty, INFINITE);
        for (size_t j = 0; j < 2; j++)
        {
            EnterCriticalSection(&cs);
            GetWindowText(hwndBuffer[j], tempBuffer, 2);
            if (!lstrcmp(tempBuffer, TEXT("0")))
            {
                memset(tempBuffer, 0, 2);
                memcpy(tempBuffer, &resourceBuffer[i], 1);
                SetWindowText(hwndBuffer[j], tempBuffer);
                LeaveCriticalSection(&cs);
                break;
            }
            LeaveCriticalSection(&cs);
        }
        //����֪ͨ
        Sleep(1000);
        ReleaseSemaphore(semaphoreFull, 1, NULL);
    }

    return 0;
}

DWORD WINAPI ClientThread(LPVOID lpParameter)
{
    DWORD index = (DWORD)lpParameter;
    DWORD retValue = 0;
    TCHAR textBuffer[100] = { 0 };
    TCHAR tempBuffer[2] = { 0 };
    TCHAR textCat[3];

    while (true)
    {
        retValue = WaitForSingleObject(semaphoreFull, 5000);
        if (retValue == WAIT_TIMEOUT)
        {
            return -1;
        }
        EnterCriticalSection(&cs);
        for (size_t i = 0; i < 2; i++)
        {
            
            //������������
            GetWindowText(hwndBuffer[i], tempBuffer, 2);
            if (lstrcmp(tempBuffer, TEXT("0")))
            {
                //��ȡԭʼ�ַ���
                GetWindowText(handleEdit[index], textBuffer, 100);
                //��ȡ�����ַ�ǰ�� ��-��
                memset(textCat, 0, 3);
                lstrcpy(textCat, TEXT("-"));
                lstrcat(textCat, tempBuffer);
                //���ӵ�ԭ�ַ�����
                lstrcat(textBuffer, textCat);
                //��ӵ��Լ���������
                SetWindowText(handleEdit[index], textBuffer);
                //���û���������Ϊ ��0��
                Sleep(600);
                SetWindowText(hwndBuffer[i], TEXT("0"));
                //LeaveCriticalSection(&cs);
                break;
            }
            
        }
        LeaveCriticalSection(&cs);
        Sleep(1000);
        ReleaseSemaphore(semaphoreEmpty, 1, NULL);
    }
    return 0;
}


