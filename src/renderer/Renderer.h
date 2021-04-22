#pragma once
//d3d11
#include "core/InitWindow.h"
#include "renderer/ShaderLibrary.h"
#include "scene/Scene.h"
#include "scene/Components.h"
#include "scene/Systems.h"

using namespace Microsoft::WRL;
using namespace DirectX;

namespace DxEngine {
	namespace SceneNS {
		class Scene;
		class Systems;
	}
	class Window;
	class Time;
	class ShaderLibrary;
	struct ConstantBufferPerRender {
		XMMATRIX projection;
		XMFLOAT4 colorLight;
		XMFLOAT3 directionLight;
		XMFLOAT2 time;
	};
	struct ConstantBufferPerFrame {
		XMMATRIX view;
	};
	struct ConstantBufferPerObject {
		XMMATRIX world;
	};
	struct BonesCB {
		XMMATRIX bones[500];
	};
	struct RenderPrimitives {
		Model box;
	};
	class RenderStates {
	public:
		//std::vector<ComPtr<ID3D11Buffer>> VBA;
		//std::vector<ComPtr<ID3D11Buffer>> IBA;
		ComPtr<ID3D11Buffer> cbPerRender;
		ComPtr<ID3D11Buffer> cbPerFrame;
		ComPtr<ID3D11Buffer> cbPerObject;
		ComPtr<ID3D11Buffer> constantBufferBones;
		ComPtr<ID3D11Texture2D> depthBuffer;
		ComPtr<ID3D11DepthStencilView> depthStencilView;
		ComPtr<ID3D11DepthStencilState> depthStencilState;
		ComPtr<ID3D11RasterizerState> rasterizerState;
		ComPtr<ID3D11RasterizerState> rasterizerState_nbf;
		//ID3D11DepthStencilState* g_depthStencilStateOff = NULL;
		ComPtr<ID3D11SamplerState> samplerState;
		//ID3D11BlendState* g_blendState = NULL;
	};
	class Renderer {
	public:
		Renderer(Window&);
		void RenderFrame();
		Window& windowRef;
		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> deviceContext;	
		ComPtr<IDXGISwapChain> swapChain;
		ComPtr<ID3D11RenderTargetView> mainRenderTargetView;

		RenderPrimitives primitives;

		//cycle buffers
		ConstantBufferPerObject cbPerObject;
		ConstantBufferPerFrame cbPerFrame;
		ConstantBufferPerRender cbPerRender;
		std::unique_ptr<ImGuiIO> guiContext;
		std::unique_ptr<RenderStates> renderStates;
		std::unique_ptr<ShaderLibrary> shaderLibrary;
		std::unique_ptr<SceneNS::Scene> currentScene;
		std::unique_ptr<SceneNS::Systems> componentSystems;
	private:
		void CreateConstantBuffers();
		void CreateDepthBuffer();
		void CreateSamplerState();
		void CreateBlendState();
		void CreateSwapChain();
		void CreateRenderTarget();
		void CreateViewport();
		void CreateImGUIContext();
		void CreateCamera();
		void CreateScene();
		void InitRenderStates();
		void CreateRasterizerState();

		void CreateBoxBuffers();
	};
}