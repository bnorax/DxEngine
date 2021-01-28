﻿#define no_init_all deprecated

#include <DirectXmath.h>
#include <DirectXColors.h>
#include <d3d11.h>
#include <tchar.h>
#include "Init.h"
#include <vector>
#include <math.h>
#include <algorithm>
#include <numeric>
#include <functional>
#include <windowsx.h>
#include <WICTextureLoader.h>
#include <chrono>
#include <time.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//win32
static TCHAR szWindowClass[] = _T("DesktopApp");
static TCHAR szTitle[] = _T("Windows Desktop Application");

std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
std::unique_ptr<DirectX::SpriteFont> spriteFont;

DirectX::XMFLOAT4 g_colorLight = { 1.0f, 1.0f, 1.0f, 1.0f };
DirectX::XMFLOAT3 g_directionLight = { -1.0f, 0 , 0.8f };

ID3D11BlendState* g_blds;
ID3D11ShaderResourceView* g_text_view = nullptr;

void Camera(POINT mPrev) {
	POINT mCur;
	float a = 1;
	if (GetAsyncKeyState(VK_RBUTTON) < 0) {
		GetCursorPos(&mCur);
		ScreenToClient(g_hWnd, &mCur);
		if (&mPrev || &mCur != NULL) {
			if (mCur.y - mPrev.y >1) {
				g_View *= DirectX::XMMatrixRotationX(-0.01f);
			}
			else if(mCur.y - mPrev.y < -1){
				g_View *= DirectX::XMMatrixRotationX(0.01f);
			}
			if (mCur.x - mPrev.x >1) {
				g_View *= DirectX::XMMatrixRotationY(-0.02f);
			}else if(mCur.x - mPrev.x < -1){
				g_View *= DirectX::XMMatrixRotationY(0.02f);
			}
		}
	}
	if (GetAsyncKeyState(VK_SHIFT) < 0) {
		a = 10;
	}
	if (GetAsyncKeyState(0x57) < 0) {
		g_View *= DirectX::XMMatrixTranslation(0, 0, -0.01f*a);
	}
	if (GetAsyncKeyState(0x53) < 0) {
		g_View *= DirectX::XMMatrixTranslation(0, 0, 0.01f*a);
	}
	if (GetAsyncKeyState(0x44) < 0) {
		g_View *= DirectX::XMMatrixTranslation(-0.01f*a, 0, 0);
	}
	if (GetAsyncKeyState(0x41) < 0) {
		g_View *= DirectX::XMMatrixTranslation(0.01f*a, 0, 0);
	}
	if (GetAsyncKeyState(0x27)<0) {
		g_directionLight.x -= 0.05f;
	}
	if (GetAsyncKeyState(0x25) < 0) {
		g_directionLight.x += 0.05f;
	}
	if (GetAsyncKeyState(0x26) < 0) {
		g_directionLight.y -= 0.05f;
	}
	if (GetAsyncKeyState(0x28) < 0) {
		g_directionLight.y += 0.05f;
	}
}

void WaveFunc(MyMesh *mesh) {
	float X, Y, Z;
	float A = 0.2f, D[2] = { 0.6f, 0.6f }, L = 1.0f, S = 0.005f;
	int t;
	float phi = S * (2/ L);
	float w = 2 / L;
	float Q = 0.05;
	for (int i = 0; i < mesh->verts.size(); i++) {
		float skl = D[0] * mesh->verts[i].pos.x + D[1] * mesh->verts[i].pos.y;
		t = GetTickCount64();
		Z = A * sin(skl*w + t * phi);
		//X = Q*A * D[0] * cos(w*skl + phi * t);
		//Y = Q*A * D[1] * cos(w*skl + phi * t);
		//mesh->verts[i].pos.x = X;
	//	mesh->verts[i].pos.y = Y;
		mesh->verts[i].pos.z = Z;

	}
}
//void meshGen(MyMesh *out) {
//	int count = 0;
//	int numSlices = 10;
//	float M_PI = 3.14;
//	int numSegments = 10;
//	int sphereRadius = 2;
//	int size = 2;
//	int posx = 0, posy = 0;
//	int x = abs(posx- size);
//	int y = abs(posy - size);
//}



void Render(ImGuiIO& io, POINT& mPrev, std::vector<MyMesh> &allObjects) {
	//imgui

	g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, DirectX::Colors::MidnightBlue);

	g_pd3dDeviceContext->ClearDepthStencilView(g_depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	static float t = 0.0f;
	static ULONGLONG timeStart = 0;
	ULONGLONG timeCur = GetTickCount64();
	if (timeStart == 0)
		timeStart = timeCur;
	t = (timeCur - timeStart) / 1000.0f;
	static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
	if (!io.WantCaptureMouse) {
		Camera(mPrev);
		GetCursorPos(&mPrev);
		ScreenToClient(g_hWnd, &mPrev);
	}
	//ImGui_ImplDX11_NewFrame();
	//ImGui_ImplWin32_NewFrame();
	//ImGui::NewFrame();
	//ImGui::Begin("Color picker");
	//ImGui::ColorPicker4("##picker", (float*)&color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
	//ImGui::End();
	//colorC.w = color.w;
	//colorC.x = color.x;
	//colorC.z = color.z;
	//colorC.y = color.y;
	//for (int i = 0; i < ARRAYSIZE(vertices); i++) {
	//	vertices[i].color = colorC;
	//}
	/*D3D11_MAPPED_SUBRESOURCE resource;
	g_pd3dDeviceContext->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, &vertices, sizeof(SimpleVertex) * ARRAYSIZE(vertices));
	g_pd3dDeviceContext->Unmap(g_pVertexBuffer, 0);
	g_pd3dDeviceContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
*/
	//DirectX::XMMATRIX rotate;
	//rotate = DirectX::XMMatrixRotationY(t);
	//DirectX::XMMATRIX translate;
	//translate = DirectX::XMMatrixTranslation(sin(t)*2.0f, 2.0f*cos(t), cos(t)*sin(t));
	//g_World = DirectX::XMMatrixRotationY(t*0.5f)  * translate;
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	g_pd3dDeviceContext->OMSetBlendState(g_blendState, blendFactor, 0xffffffff);
	g_pd3dDeviceContext->VSSetShader(vertex_shader_ptr, nullptr, 0);
	// g_pd3dDeviceContext->VSSetShader(vertex_shader_ptr, nullptr, 0);
	g_pd3dDeviceContext->VSSetConstantBuffers(1, 1, &g_pConstantBuffer);
	g_pd3dDeviceContext->PSSetConstantBuffers(1, 1, &g_pConstantBuffer);
	g_pd3dDeviceContext->IASetInputLayout(input_layout_ptr);
	g_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//g_pd3dDeviceContext->RSSetState(g_rastState);
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

	for (int i = 0; i < allObjects.size(); i++) {
		WaveFunc(&allObjects[i]);
		UpdateVertexBuffer(g_VBA.at(i), allObjects[i]);
		g_pd3dDeviceContext->IASetVertexBuffers(0, 1, &g_VBA.at(i), &stride, &offset);
		g_pd3dDeviceContext->IASetIndexBuffer(g_IBA.at(i), DXGI_FORMAT_R16_UINT, 0);
		ConstantBuffer cb;
		cb.colorLight = g_colorLight;
		cb.directionLight = g_directionLight;
		cb.time.x = GetTickCount64();
		if (allObjects[i].staticMesh) {
			//cb.mWorld = XMMatrixTranspose(g_World);
			cb.mProjection = XMMatrixTranspose(DirectX::XMMatrixOrthographicLH(16*4, 9*4, 1, 0));
			cb.mView = XMMatrixTranspose(DirectX::XMMatrixIdentity());
			cb.mWorld = XMMatrixTranspose(DirectX::XMMatrixIdentity());
			g_pd3dDeviceContext->OMSetDepthStencilState(g_depthStencilStateOff, 0);
			float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f };
			g_pd3dDeviceContext->OMSetBlendState(g_blendStateAlpha, blendFactor, 0xffffffff);

		}
		else {
			cb.mWorld = XMMatrixTranspose(g_World*DirectX::XMMatrixTranslation(0, 0, 12.0f*i));
			cb.mView = XMMatrixTranspose(g_View);
			cb.mProjection = XMMatrixTranspose(g_Projection);
			g_pd3dDeviceContext->OMSetDepthStencilState(g_depthStencilState, 0);
		}
		g_pd3dDeviceContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);
		if (allObjects[i].Texture != nullptr) {
			//g_pd3dDeviceContext->PSSetShader(pixel_wave_ptr, nullptr, 0);
			g_pd3dDeviceContext->PSSetShader(pixel_shader_ptr, nullptr, 0);
			g_pd3dDeviceContext->PSSetSamplers(1, 1, &g_SamplerState);
			g_pd3dDeviceContext->PSSetShaderResources(1, 1, &allObjects[i].Texture);
		}
		else {
			g_pd3dDeviceContext->PSSetShader(pixel_nt_shader_ptr, nullptr, 0);
		}
		g_pd3dDeviceContext->DrawIndexed(allObjects[i].ind.size(), 0, 0);
	}
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), L"PGIZ laba", DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0, DirectX::XMFLOAT2(0, 0), 2);
	spriteBatch->End();
	g_pSwapChain->Present(1, 0);
}


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	POINT mPrev;
	InitWindow(hInstance);
	ShowWindow(g_hWnd, SW_SHOWDEFAULT);
	UpdateWindow(g_hWnd);
	InitDevice(g_hWnd);

	std::vector<MyMesh> allObjects;
	MyMesh *temp = nullptr;
	HRESULT hr;
	/*hr = ReadObjFromFile("deskn.obj", &temp);
	if (hr == S_OK) {
		allObjects.push_back(*temp);
		DirectX::CreateWICTextureFromFile(g_pd3dDevice, L"desk.png", nullptr, &allObjects.back().Texture);
	}*/
	//MyMesh gen;
	//meshGen(&gen);
	//allObjects.push_back(gen);

	/*hr = ReadObjFromFile("c1.obj", &temp);
	if (hr == S_OK) {
		allObjects.push_back(*temp);
		DirectX::CreateWICTextureFromFile(g_pd3dDevice, L"c1.png", nullptr, &allObjects.back().Texture);
		
	}*/
	/*skybox = new MyMesh;
	hr = ReadObjFromFile("skybox.obj", &skybox);
	if (hr == S_OK) {
		DirectX::CreateWICTextureFromFile(g_pd3dDevice, L"skybox.png", nullptr, &skybox->Texture);
	}
*/
	
	//hr = ReadObjFromFile("scope.obj", &temp);
	//if (hr == S_OK) {
	//	/*for (int i = 0; i < temp->verts.size(); i++) {
	//		temp->verts[i].color.x = (float)rand() / ((float)RAND_MAX + 1);
	//		temp->verts[i].color.y = (float)rand() / ((float)RAND_MAX + 1);
	//		temp->verts[i].color.z = (float)rand() / ((float)RAND_MAX + 1);
	//	}*/\
	//	DirectX::CreateWICTextureFromFile(g_pd3dDevice, L"scope.png", nullptr, &temp->Texture);
	//	temp->staticMesh = true;
	//	allObjects.push_back(*temp);
	//}
	hr = ReadObjFromFile("resources/mesh/wave.obj", &temp);
	allObjects.push_back(*temp);
	InitVertexBuffer(allObjects);
	InitIndexBuffer(allObjects);
	InitDepthBuf();
	InitConstantBuffer(allObjects);
	InitShader();
	InitSamplerState();

	spriteBatch = std::make_unique<DirectX::SpriteBatch>(g_pd3dDeviceContext);
	spriteFont = std::make_unique<DirectX::SpriteFont>(g_pd3dDevice, L"resources/font/myfile.spritefont");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
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
			Render(io, mPrev, allObjects);
		}
	}

	DestroyWindow(g_hWnd);
	UnregisterClass(szWindowClass, hInstance);
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