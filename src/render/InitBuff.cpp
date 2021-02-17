#include <dxpch.h>
#include "InitBuff.h"
#include "InitDevice.h"


//ID3D11Buffer* g_pVertexBuffer = NULL;
//ID3D11Buffer* g_pIndexBuffer = NULL;
std::vector<ID3D11Buffer*> g_VBA;
std::vector<ID3D11Buffer*> g_IBA;
ID3D11Buffer* g_pConstantBuffer = NULL;
ID3D11Texture2D* g_depthBuffer = NULL;
ID3D11DepthStencilView* g_depthStencil = NULL;
ID3D11DepthStencilState* g_depthStencilState = NULL;
ID3D11DepthStencilState* g_depthStencilStateOff = NULL;
ID3D11SamplerState* g_SamplerState = NULL;

DirectX::XMMATRIX g_World;
DirectX::XMMATRIX g_View;
DirectX::XMMATRIX g_Projection;



void InitVertexBuffer(std::vector<Mesh>& allObjects) {
	for (int i = 0; i < allObjects.size(); i++) {
		ID3D11Buffer* newBuf;
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		
		size_t count = allObjects[i].vertices.size();
		bufferDesc.ByteWidth = count * sizeof(SimpleVertex);

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &allObjects[i].vertices[0];
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		g_pd3dDevice->CreateBuffer(&bufferDesc, &InitData, &newBuf);
		g_VBA.push_back(newBuf);
	}
}

void UpdateVertexBuffer(ID3D11Buffer* vb, Mesh newMesh) {
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	g_pd3dDeviceContext->Map(vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//  Update the vertex buffer here.
	memcpy(mappedResource.pData, newMesh.vertices.data() , newMesh.vertices.size()*sizeof(SimpleVertex));
	//  Reenable GPU access to the vertex buffer data.
	g_pd3dDeviceContext->Unmap(vb, 0);
}

void InitIndexBuffer(std::vector<Mesh>& allObjects) {
	for (int i = 0; i < allObjects.size(); i++) {
		ID3D11Buffer* newBuf;
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = (UINT)sizeof(WORD) * allObjects.at(i).indices.size();        // 36 vertices needed for 12 triangles in a triangle list
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &allObjects.at(i).indices[0];
		HRESULT HR = g_pd3dDevice->CreateBuffer(&bd, &InitData, &newBuf);
		g_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		g_IBA.push_back(newBuf);
	}
}

void InitConstantBuffer(std::vector<Mesh>& allObjects) {
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = 5*sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HRESULT HR = g_pd3dDevice->CreateBuffer(&bd, nullptr, &g_pConstantBuffer);
	g_pd3dDeviceContext->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);

	g_World = DirectX::XMMatrixIdentity();
	DirectX::XMVECTOR Eye = DirectX::XMVectorSet(0.0f, 2.0f, -5.0f, 0.0f);
	DirectX::XMVECTOR At = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR Up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	g_View = DirectX::XMMatrixLookAtLH(Eye, At, Up);
	g_Projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, 1280 / (FLOAT)720, 0.01f, 100.0f);
}

void InitDepthBuf() {
	D3D11_TEXTURE2D_DESC depthBuf;
	depthBuf.Width = 1280;
	depthBuf.Height = 720;
	depthBuf.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBuf.MipLevels = 1;
	depthBuf.ArraySize = 1;
	depthBuf.SampleDesc.Count = 1;
	depthBuf.SampleDesc.Quality = 0;
	depthBuf.Usage = D3D11_USAGE_DEFAULT;
	depthBuf.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBuf.CPUAccessFlags = 0;
	depthBuf.MiscFlags = 0;

	g_pd3dDevice->CreateTexture2D(&depthBuf, NULL, &g_depthBuffer);
	g_pd3dDevice->CreateDepthStencilView(g_depthBuffer, NULL, &g_depthStencil);
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, g_depthStencil);

	D3D11_DEPTH_STENCIL_DESC dDesc;
	ZeroMemory(&dDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dDesc.DepthEnable = true;
	dDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	dDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	g_pd3dDevice->CreateDepthStencilState(&dDesc, &g_depthStencilState);

	D3D11_DEPTH_STENCIL_DESC dDescOff;
	ZeroMemory(&dDescOff, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dDescOff.DepthEnable = false;
	dDescOff.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	dDescOff.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	g_pd3dDevice->CreateDepthStencilState(&dDescOff, &g_depthStencilStateOff);

}

void InitSamplerState() {
	CoInitialize(NULL);
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT hr = g_pd3dDevice->CreateSamplerState(&sampDesc, &g_SamplerState);

	//DirectX::SaveWICTextureToFile(g_pd3dDeviceContext, t ,GUID_ContainerFormatBmp, L"SCREENSHOT.BMP");
	//hr = g_pd3dDevice->CreateShaderResourceView(t, &td, &TextureTable);
	//DirectX::CreateWICTextureFromFile(g_pd3dDevice, L"SchoolDesk.png", nullptr, TextureTable);
}