#include "AudioListenerSystem.h"
#include "Audio.h"
#include "scene/components/AudioListener.h"
#include "scene/components/Transforms.h"
#include <DirectXMath.h>
#include "core/Time.h"
DxEngine::SceneNS::AudioListenerSystem::AudioListenerSystem(DirectX::AudioEngine* pAduio, entt::registry& pReg)
	: audioEngine(pAduio), registry(pReg)
{
	auto view = registry.view<Components::AudioListener>();
	for (auto entity : view) {
		Components::AudioListener& listener = registry.get<Components::AudioListener>(entity);
		listener.listener = std::make_unique<DirectX::AudioListener>();
	}
}
void DxEngine::SceneNS::AudioListenerSystem::Update()
{
	using namespace Components;
	auto view = registry.view<AudioListener>();
	for (auto entity : view) {
		AudioListener& listener = registry.get<AudioListener>(entity);
		Transforms& transform = registry.get<Transforms>(entity);
		listener.listener->SetPosition(transform.position);
	}
}
