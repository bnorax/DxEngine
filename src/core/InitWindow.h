#pragma once
#include <windows.h>
#include <tchar.h>
namespace DxEngine {
	class OSWindow {
	private:
	public:
		int wndHeight;
		int wndWidth;
		HINSTANCE hInst;
		HWND hWnd;
		const wchar_t* szWindowClass = _T("DxEngine App");
		const wchar_t* szTitle = _T("DxEngine App");
		const wchar_t* szWindowName;

		int InitWindow();
		OSWindow() {
			wndWidth = 1280;
			wndHeight = 720;
			szWindowName = _T("DxEngine Test");
		}
		OSWindow(int width, int height, const wchar_t* windowName = _T("DxEngine Test")) {
			wndWidth = width;
			wndHeight = height;
			szWindowName = windowName;
		}
	};
}