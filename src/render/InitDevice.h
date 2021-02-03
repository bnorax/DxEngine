#pragma once
int InitDevice(HWND hWnd);
void CreateRenderTarget();
void CleanupRenderTarget();
void CleanupDeviceD3D();
extern ID3D11BlendState* g_blendState;
extern ID3D11BlendState* g_blendStateAlpha;
extern ID3D11Device*            g_pd3dDevice;
extern ID3D11DeviceContext*     g_pd3dDeviceContext ;
extern IDXGISwapChain*          g_pSwapChain;
extern ID3D11RenderTargetView*  g_mainRenderTargetView;