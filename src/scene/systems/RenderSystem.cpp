#include "RenderSystem.h"
#include <DirectXColors.h>

#include <renderer/Renderer.h>
#include <scene/components/Renderable.h>

void DxEngine::SceneNS::RenderSystem::Draw(entt::registry& registry)
{

	auto view = registry.view<Components::Renderable>();
	for (auto entity : view) {
		if (view.get<Components::Renderable>(entity).render == true) {
			renderer.deviceContext->ClearRenderTargetView(renderer.mainRenderTargetView.Get(), DirectX::Colors::DarkSlateGray);
			renderer.deviceContext->ClearDepthStencilView(renderer.renderStates->depthStencilView.Get() , D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
			renderer.deviceContext->OMSetDepthStencilState(renderer.renderStates->depthStencilState.Get(), 0);

			//float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			//renderer.deviceContext->OMSetBlendState(renderer.renderStates->, blendFactor, 0xffffffff);

			auto vs = renderer.shaderLibrary->vertexShaders.at("VertexShader");
			renderer.deviceContext->VSSetShader(vs.vsPtr.Get(), nullptr, 0);
			auto ps = renderer.shaderLibrary->pixelShaders.at("textured");
			renderer.deviceContext->PSSetShader(ps.psPtr.Get(), nullptr, 0);
			renderer.deviceContext->PSSetSamplers(1, 1, renderer.renderStates.get()->samplerState.GetAddressOf());
			renderer.deviceContext->IASetInputLayout(renderer.shaderLibrary->inputLayout.Get());
			renderer.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
	}
}
