#pragma once
#include <vector>
#include <d3d11.h>
#include <directXmath.h>
#include "../src/DxEngine.h"

class MyMesh;

extern DirectX::XMMATRIX g_World;
extern DirectX::XMMATRIX g_View;
extern DirectX::XMMATRIX g_Projection;

extern ID3D11Texture2D* g_depthBuffer;
extern ID3D11DepthStencilView* g_depthStencil;
extern ID3D11DepthStencilState* g_depthStencilState;
extern ID3D11DepthStencilState* g_depthStencilStateOff;

extern ID3D11Buffer* g_pVertexBuffer;
extern ID3D11Buffer* g_pIndexBuffer;
extern ID3D11Buffer* g_pConstantBuffer;

extern ID3D11SamplerState* g_SamplerState;

extern std::vector<ID3D11Buffer*> g_VBA;

extern std::vector<ID3D11Buffer*> g_IBA;

struct ConstantBuffer {
	DirectX::XMMATRIX mWorld;
	DirectX::XMMATRIX mView;
	DirectX::XMMATRIX mProjection;
	DirectX::XMFLOAT4 colorLight;
	DirectX::XMFLOAT3 directionLight;
	DirectX::XMFLOAT2 time;
};

void InitSamplerState();
void InitDepthBuf();
void InitVertexBuffer(std::vector<MyMesh>& allObjects);
void InitIndexBuffer(std::vector<MyMesh>& allObjects);
void InitConstantBuffer(std::vector<MyMesh>& allObjects);
void UpdateVertexBuffer(ID3D11Buffer* vb, MyMesh newMesh);