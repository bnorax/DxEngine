#pragma once
#include <dxpch.h>
#include "render/Render.h"

#include <filesystem>

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
		ShaderLibrary(Render& pRender) : render(pRender) {};
		Render& render;
		
		std::wstring shadersPath;
		std::map<std::string, VertexShader> vertexShaders;
		std::map<std::string, PixelShader> pixelShaders;
		void LoadFromFile();	
	protected:
	public:
	};


}
