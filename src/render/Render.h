#pragma once
#include <dxpch.h>
#include <d3dcompiler.h>
//d3d11
#include <DirectXmath.h>
#include <DirectXColors.h>
#include "core/InitWindow.h"
#include "render/InitBuff.h"
#include "render/ShaderLibrary.h"
//#include <scene/Scene.h>

class Model;
namespace DxEngine {
	class EditorCamera;
	class Time;
	class OSWindow;

	class Render {
	public:
		Render(OSWindow&);
		//void SetScene(SceneNS::Scene &scene) {
		//	currentScene = std::make_shared<SceneNS::Scene>(scene);
		//};
		void RenderFrame(ImGuiIO&, EditorCamera&, Time&);
		OSWindow &windowRef;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout_ptr;
		//std::shared_ptr<SceneNS::Scene> currentScene;
		Microsoft::WRL::ComPtr<ID3D11Device> device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;	
	protected:
		Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mainRenderTargetView;
		Model *model;
		void loadModel();
		
	private:
		void CreateConstantBuffer();
		void CreateDepthBuffer();
		void CreateSamplerState();
		void CreateBlendState();
		void CreateSwapChain();
		void CreateRenderTarget();
		void CreateViewport();
		std::unique_ptr<ShaderLibrary> shaderLibrary(Render&);
	};
}