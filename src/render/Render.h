#pragma once
#include <dxpch.h>
#include <d3dcompiler.h>
#include <scene/Scene.h>

namespace DxEngine {

	class PixelShader {
	public:
		std::wstring path;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> psPtr = nullptr;
	};

	class VertexShader {
	public:
		std::wstring path;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> vsPtr = nullptr;

	};

	class Render {
	public:
		void loadShaders();
		DxEngine::Scene* loadScene();

		std::map<std::wstring, VertexShader> vertexShaders;
		std::map<std::wstring, PixelShader> pixelShaders;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout_ptr;
		std::unique_ptr<DxEngine::Scene> *currentScene;
		ID3D11Device *device;
		ID3D11DeviceContext *devCon;
	private:
	};
}