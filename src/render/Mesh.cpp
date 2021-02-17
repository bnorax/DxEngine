#include <dxpch.h>
#include "Mesh.h"

#include <WICTextureLoader.h>

Mesh::Mesh(ID3D11Device *dev, const std::vector<SimpleVertex> vert, const std::vector<UINT> ind, const std::vector<Texture>& tex) : 
		vertices(vert),
		indices(ind),
		textures(tex),
		device(dev)
{
	HRESULT hr;
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = static_cast<UINT>(sizeof(SimpleVertex) * vertices.size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &vertices[0];

	hr = dev->CreateBuffer(&vbd, &initData, vertexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to create vertex buffer.");
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	initData.pSysMem = &indices[0];

	hr = dev->CreateBuffer(&ibd, &initData, indexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to create index buffer.");
	}
}

void Mesh::Draw(ID3D11DeviceContext *devcon) {
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	devcon->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	for (int i = 0; i < textures.size(); i++) {
		devcon->PSSetShaderResources(i + 1, 1, textures[i].texture.GetAddressOf());
	}
	devcon->DrawIndexed(static_cast<UINT>(indices.size()), 0, 0);
}

//void Mesh::LoadTexture(const wchar_t* filename, ID3D11Device* device) {
//	ID3D11ShaderResourceView **rawTexture = new ID3D11ShaderResourceView*;
//	DirectX::CreateWICTextureFromFile(device, filename, nullptr, rawTexture);
//	texture = std::make_shared<ID3D11ShaderResourceView*>(*rawTexture);
//}