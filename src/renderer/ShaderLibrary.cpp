#include "ShaderLibrary.h"

namespace sfs = std::filesystem;

void DxEngine::ShaderLibrary::LoadFromFile()
{
	HRESULT hr;
	sfs::path workDir = sfs::current_path();
	sfs::path shaderDir = workDir / "resources" / "shaders";
	for (auto& p : sfs::directory_iterator(shaderDir / "pixel")) {
		D3DCompileFromFile(p.path().c_str(), nullptr, NULL, "ps_main", "ps_5_0", NULL, 0, &psBlob, NULL);
		PixelShader loadedShader;
		hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, loadedShader.psPtr.GetAddressOf());
		pixelShaders.insert(std::pair<std::string, PixelShader>(p.path().stem().string(), loadedShader));
	}
	for (auto& p : sfs::directory_iterator(shaderDir / "vertex")) {
		D3DCompileFromFile(p.path().c_str(), nullptr, NULL, "vs_main", "vs_5_0", NULL, 0, &vsBlob, NULL);
		VertexShader loadedShader;
		hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, loadedShader.vsPtr.GetAddressOf());
		vertexShaders.insert(std::pair<std::string, VertexShader>(p.path().stem().string(), loadedShader));
	}
}

void DxEngine::ShaderLibrary::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
		  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"BONEID", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), inputLayout.GetAddressOf());
}
