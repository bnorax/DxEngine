#include "Render.h"

namespace sfs = std::filesystem; // sfs = Std::FileSystem

void DxEngine::Render::loadShaders()
{
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
	  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{"BONEID", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{"BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	sfs::path workDir = sfs::current_path();
	sfs::path shaderDir = workDir / "resources" / "shaders";
	ID3DBlob *psBlob = nullptr, *vsBlob = nullptr;
	for (auto& p : sfs::directory_iterator(shaderDir/"pixel")) {
		D3DCompileFromFile(p.path().c_str(), nullptr, NULL, "ps_main", "ps_5_0", NULL, 0, &psBlob, NULL);
		PixelShader loadedShader;
		loadedShader.path =  p.path().c_str();
		hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, loadedShader.psPtr.GetAddressOf());
		pixelShaders.insert(std::pair<std::wstring, PixelShader>(p.path().stem(), loadedShader));
	}

	for (auto& p : sfs::directory_iterator(shaderDir / "vertex")) {
		D3DCompileFromFile(p.path().c_str(), nullptr, NULL, "vs_main", "vs_5_0", NULL, 0, &vsBlob, NULL);
		VertexShader loadedShader;
		loadedShader.path = p.path().c_str();
		hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, loadedShader.vsPtr.GetAddressOf());
		vertexShaders.insert(std::pair<std::wstring, VertexShader>(p.path().stem(), loadedShader));
	}
	hr = device->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &input_layout_ptr);
	devCon->IASetInputLayout(input_layout_ptr.Get());
}

DxEngine::Scene * DxEngine::Render::loadScene()
{

	return nullptr;
}
