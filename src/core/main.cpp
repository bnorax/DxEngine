#define no_init_all deprecated

#include "dxpch.h"
#include "render/Model.h"

//d3d11
#include <DirectXmath.h>
#include <DirectXColors.h>

//dxengine
#include <DxEngine.h>

//directxtk
#include <WICTextureLoader.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
//std::unique_ptr<DirectX::SpriteFont> spriteFont;

//DirectX::XMFLOAT4 g_colorLight = { 1.0f, 1.0f, 1.0f, 1.0f };
//DirectX::XMFLOAT3 g_directionLight = { -1.0f, 0 , 0.8f };

//ID3D11BlendState* g_blds;
//ID3D11ShaderResourceView* g_text_view = nullptr;


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	auto &timeSingl =  DxEngine::Time::Instance();
	std::unique_ptr<DxEngine::OSWindow> mainWindow = std::make_unique<DxEngine::OSWindow>(1920, 1080, _T("DxEngine App"));
	mainWindow->InitWindow();
	ShowWindow(mainWindow->hWnd, SW_SHOWDEFAULT);
	UpdateWindow(mainWindow->hWnd);

	DxEngine::Render mainRender(*mainWindow);
	mainRender.LoadShaders();
	//Camera
	DxEngine::EditorCamera camera(mainWindow->hWnd);

	//spriteBatch = std::make_unique<DirectX::SpriteBatch>(g_pd3dDeviceContext);
	//spriteFont = std::make_unique<DirectX::SpriteFont>(g_pd3dDevice, L"resources/font/myfile.spritefont");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(mainWindow->hWnd);
	ImGui_ImplDX11_Init(mainRender.device.Get(), mainRender.deviceContext.Get());
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
			mainRender.RenderFrame(io, camera, timeSingl);
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