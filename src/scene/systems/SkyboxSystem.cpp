#include <dxpch.h>
#include "SkyboxSystem.h"

void DxEngine::SceneNS::SkyboxSystem::Load()
{
}

void DxEngine::SceneNS::SkyboxSystem::Draw(entt::registry& registry)
{
	auto view = registry.view<Components::Skybox>();
	for (auto entity : view) {
		renderer.deviceContext->RSSetState(renderer.renderStates->rasterizerState_nbf.Get());
	}
}
