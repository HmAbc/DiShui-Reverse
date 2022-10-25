#include "PEDialog.h"

/// @brief ����PE��Ϣ�鿴�Ի�����Ϣ����ص�����
/// @param hwnd ���ھ��
/// @param uMsg ��Ϣ����
/// @param wParam ��һ����Ϣ����
/// @param lParam �ڶ�����Ϣ����
/// @return ��������Ϣ����TRUE��δ�������Ϣ�ύ��ϵͳ����
BOOL CALLBACK PEDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
			//�� ���� ��ťʱ���߼�
		case IDC_BUTTON_SECTION:

			return TRUE;
			//�� Ŀ¼ ��ťʱ���߼�
		case IDC_BUTTON_DIRECTORY:

			return TRUE;
		default:
			break;
		}
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;
	default:
		break;
	}
	return FALSE;
}