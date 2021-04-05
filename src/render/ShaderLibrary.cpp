#include "ShaderLibrary.h"

namespace sfs = std::filesystem;

void DxEngine::ShaderLibrary::LoadFromFile()
		HRESULT hr;
		sfs::path workDir = sfs::current_path();
		sfs::path shaderDir = workDir / "resources" / "shaders";
		for (auto& p : sfs::directory_iterator(shaderDir / "pixel")) {
			D3DCompileFromFile(p.path().c_str(), nullptr, NULL, "ps_main", "ps_5_0", NULL, 0, &psBlob, NULL);
			PixelShader loadedShader;
			hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, loadedShader.psPtr.GetAddressOf());
			shaderLibrary->pixelShaders.insert(std::pair<std::string, PixelShader>(p.path().stem().string(), loadedShader));
		}

		for (auto& p : sfs::directory_iterator(shaderDir / "vertex")) {
			D3DCompileFromFile(p.path().c_str(), nullptr, NULL, "vs_main", "vs_5_0", NULL, 0, &vsBlob, NULL);
			VertexShader loadedShader;
			hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, loadedShader.vsPtr.GetAddressOf());
			shaderLibrary->vertexShaders.insert(std::pair<std::string, VertexShader>(p.path().stem().string(), loadedShader));
		}
	}
}
