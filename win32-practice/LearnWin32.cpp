#include "Tools.h"

LRESULT CALLBACK WindowProc(
	IN  HWND hwnd,
	IN  UINT uMsg,
	IN  WPARAM wParam,
	IN  LPARAM lParam
);

int APIENTRY WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd)
{
	//TODO
    TCHAR className[] = L"First Window";

    WNDCLASS wndClass = {0};
    wndClass.lpfnWndProc = WindowProc;
    wndClass.hbrBackground = (HBRUSH)COLOR_MENU;
    wndClass.lpszClassName = className;
    wndClass.hInstance = hInstance;

    RegisterClass(&wndClass);

    HWND hwnd = CreateWindow(
        className,
        TEXT("我的第一个窗口"),
        WS_OVERLAPPEDWINDOW,
        600,
        200,
        800,
        600,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd)
    {
        return 0;
    }

    ShowWindow(hwnd, SW_SHOW);

    MSG msg;
    while (GetMessage(&msg, NULL, NULL, NULL))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	return 0;
}

LRESULT CALLBACK WindowProc(
	IN  HWND hwnd,
	IN  UINT uMsg,
	IN  WPARAM wParam,
	IN  LPARAM lParam)
{
	switch (uMsg)
	{
		//窗口消息						
		case WM_CREATE:
		{
			DbgPrintf("WM_CREATE %d %d\n", wParam, lParam);
			CREATESTRUCT* createst = (CREATESTRUCT*)lParam;
			DbgPrintf("CREATESTRUCT %s\n", createst->lpszClass);

			return 0;
		}
		case WM_MOVE:
		{
			DbgPrintf("WM_MOVE %d %d\n", wParam, lParam);
			POINTS points = MAKEPOINTS(lParam);
			DbgPrintf("X Y %d %d\n", points.x, points.y);

			return 0;
		}
		case WM_SIZE:
		{
			DbgPrintf("WM_SIZE %d %d\n", wParam, lParam);
			int newWidth = (int)(short)LOWORD(lParam);
			int newHeight = (int)(short)HIWORD(lParam);
			DbgPrintf("WM_SIZE %d %d\n", newWidth, newHeight);

			return 0;
		}
		case WM_DESTROY:
		{
			DbgPrintf("WM_DESTROY %d %d\n", wParam, lParam);
			PostQuitMessage(0);

			return 0;
		}
		//键盘消息						
		case WM_KEYUP:
		{
			DbgPrintf("WM_KEYUP %d %d\n", wParam, lParam);

			return 0;
		}
		case WM_KEYDOWN:
		{
			DbgPrintf("WM_KEYDOWN %d %d\n", wParam, lParam);

			return 0;
		}
		//鼠标消息						
		case WM_LBUTTONDOWN:
		{
			DbgPrintf("WM_LBUTTONDOWN %d %d\n", wParam, lParam);
			POINTS points = MAKEPOINTS(lParam);
			DbgPrintf("WM_LBUTTONDOWN %d %d\n", points.x, points.y);

			return 0;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}