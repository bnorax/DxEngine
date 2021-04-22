#pragma once

namespace DxEngine {
	class PixelShader {
	public:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> psPtr = nullptr;
	};

	class VertexShader {
	public:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vsPtr = nullptr;

	};
	class ShaderLibrary {
	public:
		ShaderLibrary(ID3D11Device *pDevice) : device(pDevice) {};
		std::wstring shadersPath;
		std::map<std::string, VertexShader> vertexShaders;
		std::map<std::string, PixelShader> pixelShaders;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
		void LoadFromFile();
		void CreateInputLayout();
	protected:
	private:
		ID3D11Device *device;
		Microsoft::WRL::ComPtr<ID3DBlob> psBlob, vsBlob;
	};


}
