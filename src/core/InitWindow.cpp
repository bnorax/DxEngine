#include <windows.h>
#include <tchar.h>

HINSTANCE g_hInst;
HWND g_hWnd;
int wndHeight = 720;
int wndWidth = 1280;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int InitWindow(HINSTANCE hInstance) {
	static TCHAR szWindowClass[] = _T("DesktopApp");
	static TCHAR szTitle[] = _T("Windows Desktop Guided Tour Application");

	WNDCLASSEX wcex = {
		wcex.cbSize = sizeof(WNDCLASSEX),
		wcex.style = CS_HREDRAW | CS_VREDRAW,
		wcex.lpfnWndProc = WndProc,
		wcex.cbClsExtra = 0,
		wcex.cbWndExtra = 0,
		wcex.hInstance = hInstance,
		wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION),
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
	g_hInst = hInstance;
	RECT clientArea = {0, 0, 1280, 720};
	AdjustWindowRect(&clientArea, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(szWindowClass, _T("DirectX 11 Programm"), WS_OVERLAPPEDWINDOW, 0, 0, clientArea.right - clientArea.left, clientArea.bottom - clientArea.top, NULL, NULL, hInstance, NULL);
	int a = GetLastError();
	if (!g_hWnd)
	{
		MessageBox(NULL,
			_T("Call to CreateWindow failed!"),
			_T("Windows Desktop Guided Tour"),
			NULL);

		return 1;
	}
	return 0;
}

