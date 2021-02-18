#pragma once

#include <directXmath.h>
#include <DxEngine.h>
using namespace DirectX;
class Mesh;

extern XMMATRIX g_World;
extern XMMATRIX g_View;
extern XMMATRIX g_Projection;

extern ID3D11Buffer* g_pConstantBufferBones;

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
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMFLOAT4 colorLight;
	XMFLOAT3 directionLight;
	XMFLOAT2 time;
	//XMMATRIX bones[100];
};

struct BonesCB {
	XMMATRIX bones[1000];
};

void InitSamplerState();
void InitDepthBuf();
void InitVertexBuffer(std::vector<Mesh>& allObjects);
void InitIndexBuffer(std::vector<Mesh>& allObjects);
void InitConstantBuffer(std::vector<Mesh>& allObjects);
void UpdateVertexBuffer(ID3D11Buffer* vb, Mesh newMesh);