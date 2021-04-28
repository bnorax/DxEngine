#include "AudioSystem.h"
#include "scene/components/AudioListener.h"
#include "scene/components/AudioSource.h"
#include "scene/components/Transforms.h"
using namespace DirectX;
DxEngine::SceneNS::AudioSystem::AudioSystem(Renderer& pRend, entt::registry& pReg)
	: renderer(pRend), registry(pReg)
{
	AUDIO_ENGINE_FLAGS aeflags = AudioEngine_Default;

#ifdef _DEBUG
	aeflags |= AudioEngine_Debug;
#endif
	audioEngine = std::make_unique<AudioEngine>(aeflags);
	sourceSystem = std::make_unique<AudioSourceSystem>(audioEngine.get(), pReg);
	listenerSystem = std::make_unique<AudioListenerSystem>(audioEngine.get(), pReg);
}

void DxEngine::SceneNS::AudioSystem::Update()
{
	sourceSystem->Update();
	listenerSystem->Update();
	auto viewL = registry.view<Components::AudioListener>();
	auto viewS = registry.view<Components::AudioSource>();
	for (auto lEntity : viewL) {
		Components::AudioListener& listener = registry.get<Components::AudioListener>(lEntity);
		Components::Transforms& lTransform = registry.get<Components::Transforms>(lEntity);
		for (auto sEntity : viewS) {
			Components::AudioSource& source = registry.get<Components::AudioSource>(sEntity);
			Components::Transforms& sTransform = registry.get<Components::Transforms>(sEntity);
			source.soundInstance->Apply3D(*listener.listener, *source.emitter, false);
			//source.soundInstance->Play(true);
		}
	}
	audioEngine->Update();
}
