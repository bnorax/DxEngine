#define no_init_all deprecated

#include "dxpch.h"

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

std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
std::unique_ptr<DirectX::SpriteFont> spriteFont;

DirectX::XMFLOAT4 g_colorLight = { 1.0f, 1.0f, 1.0f, 1.0f };
DirectX::XMFLOAT3 g_directionLight = { -1.0f, 0 , 0.8f };

ID3D11BlendState* g_blds;
ID3D11ShaderResourceView* g_text_view = nullptr;
HWND g_hWnd;


void WaveFunc(MyMesh *mesh) {
	float X, Y, Z;
	float A = 0.1f, D[2] = { 0.1f, 0.2f }, L = 0.5f, S = 0.01f;
	int t;
	float phi = S * (2 / L);
	float w = 2 / L;
	float Q = 0.05f;
	for (int i = 0; i < mesh->verts.size(); i++) {
		float skl = D[0] * mesh->verts[i].pos.x + D[1] * mesh->verts[i].pos.y;
		t = (int)GetTickCount64();
		Z = A * sin(skl*w + t * phi);
		//X = Q*A * D[0] * cos(w*skl + phi * t);
		//Y = Q*A * D[1] * cos(w*skl + phi * t);
		//mesh->verts[i].pos.x = X;
	//	mesh->verts[i].pos.y = Y;
		mesh->verts[i].pos.z = Z;

	}
}


void Render(ImGuiIO &io, DxEngine::EditorCamera &camera, std::vector<MyMesh> &allObjects) {
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

	//imgui mouse capture switch
	if (!io.WantCaptureMouse) {
		camera.EditorCameraUpdate();
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
		cb.time.x = (float)GetTickCount64();
		if (allObjects[i].staticMesh) {
			//cb.mWorld = XMMatrixTranspose(g_World);
			cb.mProjection = XMMatrixTranspose(DirectX::XMMatrixOrthographicLH(16 * 4, 9 * 4, 1, 0));
			cb.mView = XMMatrixTranspose(DirectX::XMMatrixIdentity());
			cb.mWorld = XMMatrixTranspose(DirectX::XMMatrixIdentity());
			g_pd3dDeviceContext->OMSetDepthStencilState(g_depthStencilStateOff, 0);
			float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			g_pd3dDeviceContext->OMSetBlendState(g_blendStateAlpha, blendFactor, 0xffffffff);

		}
		else {
			cb.mWorld = XMMatrixTranspose(g_World*DirectX::XMMatrixTranslation(0, 0, 12.0f*i));
			cb.mView = XMMatrixTranspose(g_View);
			cb.mProjection = XMMatrixTranspose(g_Projection);
			g_pd3dDeviceContext->OMSetDepthStencilState(g_depthStencilState, 0);
		}
		g_pd3dDeviceContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);
		if (allObjects[i].texture != nullptr) {
			//g_pd3dDeviceContext->PSSetShader(pixel_wave_ptr, nullptr, 0);
			g_pd3dDeviceContext->PSSetShader(pixel_shader_ptr, nullptr, 0);
			g_pd3dDeviceContext->PSSetSamplers(1, 1, &g_SamplerState);
			g_pd3dDeviceContext->PSSetShaderResources(1, 1, allObjects[i].texture.get());
		}
		else {
			g_pd3dDeviceContext->PSSetShader(pixel_nt_shader_ptr, nullptr, 0);
		}
		g_pd3dDeviceContext->DrawIndexed((UINT)allObjects[i].ind.size(), 0, 0);
	}
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), L"PGIZ laba", DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0, DirectX::XMFLOAT2(0, 0), 1);
	spriteBatch->End();
	g_pSwapChain->Present(1, 0);
}


int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	DxEngine::OSWindow mainWindow(1920, 1080,_T("MainWindow"));
	g_hWnd = mainWindow.hWnd;
	mainWindow.InitWindow();
	ShowWindow(mainWindow.hWnd, SW_SHOWDEFAULT);
	UpdateWindow(mainWindow.hWnd);
	InitDevice(mainWindow.hWnd);

	//Camera
	DxEngine::EditorCamera camera(&mainWindow.hWnd);

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

	hr = RedObjFromFileAssimp("resources/mesh/wave.obj", &temp);

	//hr = ReadObjFromFile("resources/mesh/wave.obj", &temp);
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
	ImGui_ImplWin32_Init(mainWindow.hWnd);
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
			Render(io, camera, allObjects);
		}
	}

	DestroyWindow(mainWindow.hWnd);
	UnregisterClass(mainWindow.szWindowClass, hInstance);
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