#include "IatDialog.h"

/// @brief IAT �Ի�����Ϣ������
/// @param hDlg �Ի�����
/// @param uMsg ��Ϣ���
/// @param wParam ��Ϣ��һ������
/// @param lParam ��Ϣ�ڶ�������
/// @return ������Ϣ����󷵻�TRUE��δ������FALSE������ϵͳ����
BOOL CALLBACK IatDialogProc(
	HWND hDlg,		// handle of window
	UINT uMsg,		// message identifier
	WPARAM wParam,	// first message parameter
	LPARAM lParam	// second message parameter
)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		InitIatDialog(hDlg);
		return TRUE;

	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return TRUE;

	default:
		break;
	}
	return FALSE;
}

/// @brief ��ʼ��IAT��Ի����е�����
/// @param hDlg Ŀ¼�Ի���ľ��
/// @return �޴��󷵻�0���д��󷵻ش����룬1000+
DWORD InitIatDialog(IN HWND hDlg)
{
	HWND iatEdit = GetDlgItem(hDlg, IDC_EDIT_IAT);

	SendMessage(iatEdit, EM_REPLACESEL, FALSE, (LPARAM)TEXT("  ���ˣ���д��"));
	return 0;
}