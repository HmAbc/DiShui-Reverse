#include "Utils.h"
#include "MainDialog.h"

HINSTANCE hAppInstance;
TCHAR fileName[256];

int APIENTRY WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
)
{
    //注册所有通用控件
    //INITCOMMONCONTROLSEX icex;
    //icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    //icex.dwICC = ICC_WIN95_CLASSES;
    //InitCommonControlsEx(&icex);

    //DbgPrintf("%d\n", ix);
    hAppInstance = hInstance;
    DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, MainDialogProc);

    return 0;
}

