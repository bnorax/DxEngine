#include <dxpch.h>
#include "InitWindow.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int DxEngine::OSWindow::InitWindow() {
	WNDCLASSEX wcex = {
		wcex.cbSize = sizeof(WNDCLASSEX),
		wcex.style = CS_HREDRAW | CS_VREDRAW,
		wcex.lpfnWndProc = WndProc,
		wcex.cbClsExtra = 0,
		wcex.cbWndExtra = 0,
		wcex.hInstance = hInst,
		wcex.hIcon = LoadIcon(hInst, IDI_APPLICATION),
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW),
		wcex.hbrBackground = NULL,
		wcex.lpszMenuName = NULL,
		wcex.lpszClassName = szWindowClass,
		wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION) 
	};
	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
			_T("Call to RegisterClassEx failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}
	RECT clientArea = {0, 0, wndWidth, wndHeight};
	AdjustWindowRect(&clientArea, WS_OVERLAPPEDWINDOW, FALSE);
	hWnd = CreateWindow(szWindowClass, szWindowName, WS_OVERLAPPEDWINDOW, 0, 0, clientArea.right - clientArea.left, clientArea.bottom - clientArea.top, NULL, NULL, hInst, NULL);
	int a = GetLastError();
	if (!hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}
	return 0;
}