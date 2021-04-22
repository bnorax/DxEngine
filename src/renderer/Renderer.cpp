#include "Renderer.h"
#include <DirectXColors.h>

namespace sfs = std::filesystem;

void DxEngine::Renderer::CreateConstantBuffers()
{
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBufferPerRender);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HRESULT HR = device->CreateBuffer(&bd, nullptr, renderStates->cbPerRender.GetAddressOf());

	D3D11_BUFFER_DESC bd1 = {};
	bd1.Usage = D3D11_USAGE_DEFAULT;
	bd1.ByteWidth = sizeof(ConstantBufferPerObject);
	bd1.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd1.CPUAccessFlags = 0;
	HR = device->CreateBuffer(&bd1, nullptr, renderStates->cbPerObject.GetAddressOf());

	D3D11_BUFFER_DESC bd2 = {};
	bd2.Usage = D3D11_USAGE_DEFAULT;
	bd2.ByteWidth = sizeof(BonesCB);
	bd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd2.CPUAccessFlags = 0;
	HR = device->CreateBuffer(&bd2, nullptr, renderStates->constantBufferBones.GetAddressOf());

	D3D11_BUFFER_DESC bd3 = {};
	bd3.Usage = D3D11_USAGE_DEFAULT;
	bd3.ByteWidth = sizeof(ConstantBufferPerFrame);
	bd3.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd3.CPUAccessFlags = 0;
	HR = device->CreateBuffer(&bd3, nullptr, renderStates->cbPerFrame.GetAddressOf());
}

void DxEngine::Renderer::CreateBoxBuffers()
{
	HRESULT hr;
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = static_cast<UINT>(sizeof(Vertex) * primitives.box.vertices.size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &primitives.box.vertices[0];

	hr = device->CreateBuffer(&vbd, &initData, primitives.box.vertexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to create vertex buffer.");
	}
}

void DxEngine::Renderer::CreateDepthBuffer()
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

	HRESULT hr = device->CreateTexture2D(&depthBuf, NULL, renderStates->depthBuffer.GetAddressOf());
	device->CreateDepthStencilView(renderStates->depthBuffer.Get(), NULL, renderStates->depthStencilView.GetAddressOf());
	deviceContext->OMSetRenderTargets(1, mainRenderTargetView.GetAddressOf(), renderStates->depthStencilView.Get());

	D3D11_DEPTH_STENCIL_DESC dDesc;
	ZeroMemory(&dDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dDesc.DepthEnable = true;
	dDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	dDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&dDesc, renderStates->depthStencilState.GetAddressOf());

	/*D3D11_DEPTH_STENCIL_DESC dDescOff;
	ZeroMemory(&dDescOff, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dDescOff.DepthEnable = false;
	dDescOff.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	dDescOff.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	device->CreateDepthStencilState(&dDescOff, &g_depthStencilStateOff);*/
}

void DxEngine::Renderer::CreateSamplerState()
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
	HRESULT hr = device->CreateSamplerState(&sampDesc, renderStates->samplerState.GetAddressOf());

	//SaveWICTextureToFile(g_pd3dDeviceContext, t ,GUID_ContainerFormatBmp, L"SCREENSHOT.BMP");
	//hr = g_pd3dDevice->CreateShaderResourceView(t, &td, &TextureTable);
	//CreateWICTextureFromFile(g_pd3dDevice, L"SchoolDesk.png", nullptr, TextureTable);
}

void DxEngine::Renderer::CreateBlendState()
{
	/*D3D11_BLEND_DESC bs;
	ZeroMemory(&bs, sizeof(bs));
	bs.RenderTarget[0].BlendEnable = false;
	bs.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&bs, renderStates->b);*/
}

void DxEngine::Renderer::CreateSwapChain()
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

void DxEngine::Renderer::CreateRenderTarget()
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	swapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
	device->CreateRenderTargetView(backBuffer.Get(), NULL, mainRenderTargetView.GetAddressOf());
	deviceContext->OMSetRenderTargets(1, mainRenderTargetView.GetAddressOf(), NULL);
}

void DxEngine::Renderer::CreateViewport()
{
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<float>(windowRef.width);
	vp.Height = static_cast<float>(windowRef.height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	deviceContext->RSSetViewports(1, &vp);
}

void DxEngine::Renderer::CreateImGUIContext()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	guiContext = std::make_unique<ImGuiIO>(ImGui::GetIO());
	//(void)guiContext;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(windowRef.hWnd);
	ImGui_ImplDX11_Init(device.Get(), deviceContext.Get());
}

void DxEngine::Renderer::CreateCamera()
{
	//camera = std::make_unique<EditorCamera>(windowRef.hWnd);
}

void DxEngine::Renderer::CreateScene()
{
	currentScene = std::make_unique<SceneNS::Scene>();
}

void DxEngine::Renderer::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC rasterizerState;
	ZeroMemory(&rasterizerState, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerState.FillMode = D3D11_FILL_SOLID;
	rasterizerState.CullMode = D3D11_CULL_BACK;
	device->CreateRasterizerState(&rasterizerState, renderStates->rasterizerState.GetAddressOf());

	D3D11_RASTERIZER_DESC rasterizerState1;
	ZeroMemory(&rasterizerState1, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerState1.FillMode = D3D11_FILL_SOLID;
	rasterizerState1.CullMode = D3D11_CULL_NONE;

	device->CreateRasterizerState(&rasterizerState1, renderStates->rasterizerState_nbf.GetAddressOf());
}

void DxEngine::Renderer::InitRenderStates()
{
}

DxEngine::Renderer::Renderer(Window &window) : windowRef(window)
{
	using namespace SceneNS::Components;
	CreateSwapChain();
	CreateRenderTarget();
	CreateViewport();
	renderStates = std::make_unique<RenderStates>();
	CreateDepthBuffer();
	CreateConstantBuffers();
	CreateSamplerState();
	CreateRasterizerState();
	CreateImGUIContext();

	shaderLibrary = std::make_unique<ShaderLibrary>(device.Get());
	shaderLibrary->LoadFromFile();
	shaderLibrary->CreateInputLayout();

	deviceContext->VSSetConstantBuffers(0, 1, renderStates->cbPerRender.GetAddressOf());
	deviceContext->VSSetConstantBuffers(1, 1, renderStates->cbPerObject.GetAddressOf());
	deviceContext->VSSetConstantBuffers(2, 1, renderStates->constantBufferBones.GetAddressOf());
	deviceContext->VSSetConstantBuffers(3, 1, renderStates->cbPerFrame.GetAddressOf());

	XMVECTOR Eye = XMVectorSet(0.0f, 2.0f, -5.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	cbPerFrame.view = XMMatrixLookAtLH(Eye, At, Up);
	cbPerRender.projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, 1920 / (FLOAT)1080, 0.01f, 100.0f);

	ConstantBufferPerRender rcb = cbPerRender;
	rcb.projection = XMMatrixTranspose(rcb.projection);
	ConstantBufferPerFrame cb = cbPerFrame;
	cb.view = XMMatrixTranspose(cb.view);
	deviceContext->UpdateSubresource(renderStates->cbPerRender.Get(), 0, nullptr, &rcb, 0, 0);
	deviceContext->UpdateSubresource(renderStates->cbPerFrame.Get(), 0, nullptr, &cb, 0, 0);

	CreateScene();

	auto entity = currentScene->registry.create();
	currentScene->registry.emplace<Camera>(entity);
	currentScene->registry.emplace<Renderable>(entity);
	currentScene->registry.emplace<Model>(entity, "resources\\mesh\\fox.glb");
	BoxCollider collider;
	collider.colliderBox.Center = { 0.0f,  0.0f, 0.0f };
	collider.colliderBox.Extents = { 2.0f,  2.0f,  2.0f };
	currentScene->registry.emplace<BoxCollider>(entity, collider);

	auto skyboxEntity = currentScene->registry.create();
	currentScene->registry.emplace<Renderable>(skyboxEntity);
	currentScene->registry.emplace<Model>(skyboxEntity, std::string("resources\\mesh\\skybox.glb"));
	currentScene->registry.emplace<Skybox>(skyboxEntity);

	componentSystems = std::make_unique<SceneNS::Systems>(*this, *currentScene.get());
	componentSystems->InitSystems();
	componentSystems->modelSystem->Load(currentScene->registry);
	//auto model = currentScene->registry.get<SceneNS::Components::Model>(entity);
	//loadModel();
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

void DxEngine::Renderer::RenderFrame()
{
	componentSystems->UpdateFrame();

	swapChain->Present(0, 0);
	//imgui

//	deviceContext->ClearRenderTargetView(mainRenderTargetView.Get(), DirectX::Colors::DarkSlateGray);
//	deviceContext->ClearDepthStencilView(g_depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
//	//static float t = 0.0f;
//	//static ULONGLONG timeStart = 0;
//	//ULONGLONG timeCur = GetTickCount64();
//	//if (timeStart == 0)
//	//	timeStart = timeCur;
//	//t = (timeCur - timeStart) / 1000.0f;
//	//static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
//
//	//imgui mouse capture switch
//	if (!guiContext->WantCaptureMouse) {
//		camera->EditorCameraUpdate();
//	}
//	/*ImGui_ImplDX11_NewFrame();
//	ImGui_ImplWin32_NewFrame();
//	ImGui::NewFrame();
//	ImGui::Begin("Color picker");
//	ImGui::ColorPicker4("##picker", (float*)&color, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
//	ImGui::End();
//	colorC.w = color.w;
//	colorC.x = color.x;
//	colorC.z = color.z;
//	colorC.y = color.y;
//	for (int i = 0; i < ARRAYSIZE(vertices); i++) {
//		vertices[i].color = colorC;
//	}*/
//	//D3D11_MAPPED_SUBRESOURCE resource;
//	//dev->Map(g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
//	//memcpy(resource.pData, &vertices, sizeof(SimpleVertex) * ARRAYSIZE(vertices));
//	//g_pd3dDeviceContext->Unmap(g_pVertexBuffer, 0);
//	//g_pd3dDeviceContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
//
//	/*DirectX::XMMATRIX rotate;
//	rotate = DirectX::XMMatrixRotationY(t);
//	DirectX::XMMATRIX translate;
//	translate = DirectX::XMMatrixTranslation(sin(t)*2.0f, 2.0f*cos(t), cos(t)*sin(t));
//	g_World = DirectX::XMMatrixRotationY(t*0.5f)  * translate;*/
//
//	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
//	deviceContext->OMSetBlendState(g_blendState, blendFactor, 0xffffffff);
//
//	deviceContext->OMSetDepthStencilState(g_depthStencilState, 0);
//
//	auto vs = shaderLibrary->vertexShaders.at("VertexShader");
//	deviceContext->VSSetShader(vs.vsPtr.Get(), nullptr, 0);
//	deviceContext->VSSetConstantBuffers(1, 1, &g_pConstantBuffer);
//	// g_pd3dDeviceContext->VSSetShader(vertex_shader_ptr, nullptr, 0);
//	auto ps = shaderLibrary->pixelShaders.at("textured");
//	deviceContext->PSSetShader(ps.psPtr.Get(), nullptr, 0);
//	deviceContext->PSSetConstantBuffers(1, 1, &g_pConstantBuffer);
//	deviceContext->PSSetSamplers(1, 1, &g_SamplerState);
//;
//	deviceContext->IASetInputLayout(shaderLibrary->inputLayout.Get());
//	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	//model->draw(deviceContext.Get());
//
//	/*spriteBatch->Begin();
//	spriteFont->DrawString(spriteBatch.get(), std::to_string(timeSingl.GetFps()).c_str(), DirectX::XMFLOAT2(0, 0), DirectX::Colors::White, 0, DirectX::XMFLOAT2(0, 0), 1);
//	spriteBatch->End();*/

	//swapChain->Present(0, 0);
}


//DxEngine::Scene * DxEngine::Render::loadScene()
//{
//
//	return nullptr;
//}
