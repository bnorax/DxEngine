#pragma once
namespace DxEngine {
	class Window {
	private:
	public:
		int width = 1280;
		int height = 720;
		//HINSTANCE hInst;
		HWND hWnd;
		const wchar_t* szWindowClass = _T("DxEngine App");
		const wchar_t* szTitle = _T("DxEngine App");
		const wchar_t* szWindowName;

		int InitWindow();
		Window() = default;
		Window(int pWidth, int pHeight, const wchar_t* windowName = _T("DxEngine Test")) {
			width = pWidth;
			height = pHeight;
			szWindowName = windowName;
		}
		~Window() {
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			//UnregisterClass(szWindowClass, hInst);??
			//CloseHandle(hInst);
		}
	};
}