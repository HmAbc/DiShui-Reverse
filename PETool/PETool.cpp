#include "Utils.h"
#include "MainDialog.h"

HINSTANCE hAppInstance;

int APIENTRY WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
)
{
    //ע������ͨ�ÿؼ�
    //INITCOMMONCONTROLSEX icex;
    //icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    //icex.dwICC = ICC_WIN95_CLASSES;
    //InitCommonControlsEx(&icex);

    hAppInstance = hInstance;
    DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, MainDialogProc);

    return 0;
}

