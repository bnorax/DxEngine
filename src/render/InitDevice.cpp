#include <dxpch.h>
#include "InitDevice.h"
//#include "RenderTriangle.h"


ID3D11Device*            g_pd3dDevice = NULL;
ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
IDXGISwapChain*          g_pSwapChain = NULL;
ID3D11RenderTargetView*  g_mainRenderTargetView = NULL;
ID3D11BlendState* g_blendState = NULL;
ID3D11BlendState* g_blendStateAlpha = NULL;

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}



int InitDevice(HWND hWnd) {
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 1920;
	sd.BufferDesc.Height = 1080;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 4;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
		return 1;
	CreateRenderTarget();
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)1920;
	vp.Height = (FLOAT)1080;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	g_pd3dDeviceContext->RSSetViewports(1, &vp);

	D3D11_BLEND_DESC bs;
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
	g_pd3dDevice->CreateBlendState(&bs2, &g_blendStateAlpha);

	return 0;
}