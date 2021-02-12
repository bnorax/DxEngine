#include <dxpch.h>
#include "ObjectToRender.h"

#include <WICTextureLoader.h>

void MyMesh::LoadTexture(const wchar_t* filename, ID3D11Device* device) {
	ID3D11ShaderResourceView **rawTexture = new ID3D11ShaderResourceView*;
	DirectX::CreateWICTextureFromFile(device, filename, nullptr, rawTexture);
	texture = std::make_shared<ID3D11ShaderResourceView*>(*rawTexture);
}