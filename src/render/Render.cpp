#include "Render.h"

namespace sfs = std::filesystem;

void DxEngine::Render::loadModel()
{
	model = new Model();
	bool hey = model->load(windowRef.hWnd, device.Get(), deviceContext.Get(), std::string("C:\\Engine\\DxEngine\\resources\\mesh\\miku.glb"));
}

void DxEngine::Render::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HRESULT HR = device->CreateBuffer(&bd, nullptr, &g_pConstantBuffer);
	deviceContext->VSSetConstantBuffers(1, 1, &g_pConstantBuffer);

	D3D11_BUFFER_DESC bd1 = {};
	bd1.Usage = D3D11_USAGE_DEFAULT;
	bd1.ByteWidth = sizeof(BonesCB);
	bd1.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd1.CPUAccessFlags = 0;
	HR = device->CreateBuffer(&bd1, nullptr, &g_pConstantBufferBones);
	deviceContext->VSSetConstantBuffers(2, 1, &g_pConstantBufferBones);

	g_World = XMMatrixIdentity();
	XMVECTOR Eye = XMVectorSet(0.0f, 2.0f, -5.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_View = XMMatrixLookAtLH(Eye, At, Up);
	g_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, 1920 / (FLOAT)1080, 0.01f, 100.0f);
}

void DxEngine::Render::CreateDepthBuffer()
{
	D3D11_TEXTURE2D_DESC depthBuf;
	depthBuf.Width = windowRef.width;
	depthBuf.Height = windowRef.height;
	depthBuf.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBuf.MipLevels = 1;
	depthBuf.ArraySize = 1;
	depthBuf.SampleDesc.Count = 4;
	depthBuf.SampleDesc.Quality = 0;
	depthBuf.Usage = D3D11_USAGE_DEFAULT;
	depthBuf.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBuf.CPUAccessFlags = 0;
	depthBuf.MiscFlags = 0;

	device->CreateTexture2D(&depthBuf, NULL, &g_depthBuffer);
	device->CreateDepthStencilView(g_depthBuffer, NULL, &g_depthStencil);
	deviceContext->OMSetRenderTargets(1, mainRenderTargetView.GetAddressOf(), g_depthStencil);

	D3D11_DEPTH_STENCIL_DESC dDesc;
	ZeroMemory(&dDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dDesc.DepthEnable = true;
	dDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	dDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&dDesc, &g_depthStencilState);

	D3D11_DEPTH_STENCIL_DESC dDescOff;
	ZeroMemory(&dDescOff, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dDescOff.DepthEnable = false;
	dDescOff.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	dDescOff.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&dDescOff, &g_depthStencilStateOff);
}

void DxEngine::Render::CreateSamplerState()
{
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr = device->CreateSamplerState(&sampDesc, &g_SamplerState);

	//SaveWICTextureToFile(g_pd3dDeviceContext, t ,GUID_ContainerFormatBmp, L"SCREENSHOT.BMP");
	//hr = g_pd3dDevice->CreateShaderResourceView(t, &td, &TextureTable);
	//CreateWICTextureFromFile(g_pd3dDevice, L"SchoolDesk.png", nullptr, TextureTable);
}

void DxEngine::Render::CreateBlendState()
{
	D3D11_BLEND_DESC bs;
	ZeroMemory(&bs, sizeof(bs));
	bs.RenderTarget[0].BlendEnable = false;
	bs.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&bs, &g_blendState);
}

void DxEngine::Render::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = windowRef.width;
	sd.BufferDesc.Height = windowRef.height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = windowRef.hWnd;
	sd.SampleDesc.Count = 4;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	//Create success check for sc creation +++
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, swapChain.GetAddressOf(), device.GetAddressOf(), &featureLevel, deviceContext.GetAddressOf());
}

void DxEngine::Render::CreateRenderTarget()
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
	device->CreateRenderTargetView(backBuffer.Get(), NULL, mainRenderTargetView.GetAddressOf());
	deviceContext->OMSetRenderTargets(1, mainRenderTargetView.GetAddressOf(), NULL);
}

void DxEngine::Render::CreateViewport()
{
	D3D11_VIEWPORT vp;
	vp.Width = windowRef.width;
	vp.Height = windowRef.height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	deviceContext->RSSetViewports(1, &vp);
}

DxEngine::Render::Render(OSWindow &window) : windowRef(window)
{
	CreateSwapChain();
	CreateRenderTarget();
	CreateViewport();
	CreateDepthBuffer();
	CreateConstantBuffer();
	CreateSamplerState();
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
		  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"BONEID", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	ID3DBlob *psBlob = nullptr, *vsBlob = nullptr;
	device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &input_layout_ptr);
	deviceContext->IASetInputLayout(input_layout_ptr.Get());
	loadModel();
	/*D3D11_BLEND_DESC bs;
	ZeroMemory(&bs, sizeof(bs));
	bs.RenderTarget[0].BlendEnable = false;
	bs.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	g_pd3dDevice->CreateBlendState(&bs, &g_blendState);

	D3D11_BLEND_DESC bs2;
	ZeroMemory(&bs2, sizeof(bs2));
	bs2.RenderTarget[0].BlendEnable = true;
	bs2.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bs2.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bs2.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bs2.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bs2.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bs2.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bs2.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	g_pd3dDevice->CreateBlendState(&bs2, &g_blendStateAlpha);*/

}

void DxEngine::Render::LoadShaders()
{
	shaderLibrary = std::make_unique<ShaderLibrary>();
	HRESULT hr;
	sfs::path workDir = sfs::current_path();
	sfs::path shaderDir = workDir / "resources" / "shaders";
	for (auto& p : sfs::directory_iterator(shaderDir/"pixel")) {
		D3DCompileFromFile(p.path().c_str(), nullptr, NULL, "ps_main", "ps_5_0", NULL, 0, &psBlob, NULL);
		PixelShader loadedShader;
		hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, loadedShader.psPtr.GetAddressOf());
		shaderLibrary->pixelShaders.insert(std::pair<std::string, PixelShader>(p.path().stem().string(), loadedShader));
	}

	for (auto& p : sfs::directory_iterator(shaderDir / "vertex")) {
		D3DCompileFromFile(p.path().c_str(), nullptr, NULL, "vs_main", "vs_5_0", NULL, 0, &vsBlob, NULL);
		VertexShader loadedShader;
		hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, loadedShader.vsPtr.GetAddressOf());
		shaderLibrary->vertexShaders.insert(std::pair<std::string, VertexShader>(p.path().stem().string(), loadedShader));
	}
}



void DxEngine::Render::RenderFrame(ImGuiIO& io, EditorCamera& camera, Time& timeSingl)
{
	//imgui

	deviceContext->ClearRenderTargetView(mainRenderTargetView.Get(), DirectX::Colors::DarkSlateGray);
	deviceContext->ClearDepthStencilView(g_depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//static float t = 0.0f;
	//static ULONGLONG timeStart = 0;
	//ULONGLONG timeCur = GetTickCount64();
	//if (timeStart == 0)
	//	timeStart = timeCur;
	//t = (timeCur - timeStart) / 1000.0f;
	//static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);

	//imgui mouse capture switch
	if (!io.WantCaptureMouse) {
		camera.EditorCameraUpdate();
	}
	/*ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Color picker");
	ImGui::ColorPicker4("##picker", (float*)&color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
	ImGui::End();
	colorC.w = color.w;
	colorC.x = color.x;
	colorC.z = color.z;
	colorC.y = color.y;
	for (int i = 0; i < ARRAYSIZE(vertices); i++) {
		vertices[i].color = colorC;
	}*/
	//D3D11_MAPPED_SUBRESOURCE resource;
	//dev->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	//memcpy(resource.pData, &vertices, sizeof(SimpleVertex) * ARRAYSIZE(vertices));
	//g_pd3dDeviceContext->Unmap(g_pVertexBuffer, 0);
	//g_pd3dDeviceContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	/*DirectX::XMMATRIX rotate;
	rotate = DirectX::XMMatrixRotationY(t);
	DirectX::XMMATRIX translate;
	translate = DirectX::XMMatrixTranslation(sin(t)*2.0f, 2.0f*cos(t), cos(t)*sin(t));
	g_World = DirectX::XMMatrixRotationY(t*0.5f)  * translate;*/

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	deviceContext->OMSetBlendState(g_blendState, blendFactor, 0xffffffff);

	deviceContext->OMSetDepthStencilState(g_depthStencilState, 0);

	auto vs = vertexShaders.at(L"VertexShader");
	deviceContext->VSSetShader(vs.vsPtr.Get(), nullptr, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &g_pConstantBuffer);
	// g_pd3dDeviceContext->VSSetShader(vertex_shader_ptr, nullptr, 0);
	auto ps = pixelShaders.at(L"textured");
	deviceContext->PSSetShader(ps.psPtr.Get(), nullptr, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &g_pConstantBuffer);
	deviceContext->PSSetSamplers(1, 1, &g_SamplerState);
;
	deviceContext->IASetInputLayout(input_layout_ptr.Get());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	model->draw(deviceContext.Get());

	/*spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), std::to_string(timeSingl.GetFps()).c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0, DirectX::XMFLOAT2(0, 0), 1);
	spriteBatch->End();*/
	swapChain->Present(0, 0);
}


//DxEngine::Scene * DxEngine::Render::loadScene()
//{
//
//	return nullptr;
//}
