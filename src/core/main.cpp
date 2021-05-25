#include "dxpch.h"
//dxengine
#include "core/InitWindow.h"
#include "core/Time.h"
#include "renderer/Renderer.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
//std::unique_ptr<DirectX::SpriteFont> spriteFont;

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	auto& timeSingl = DxEngine::Time::Instance();
	std::unique_ptr<DxEngine::Window> mainWindow = std::make_unique<DxEngine::Window>(1280, 720, _T("DxEngine App"));
	mainWindow->InitWindow();
	ShowWindow(mainWindow->hWnd, SW_SHOWDEFAULT);
	UpdateWindow(mainWindow->hWnd);

	DxEngine::Renderer mainRender(*mainWindow);

	MSG msg = {};
	ZeroMemory(&msg, sizeof(msg));
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		else {
			timeSingl.FrameBegin();
			mainRender.RenderFrame();
			timeSingl.FrameEnd();
		}
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}