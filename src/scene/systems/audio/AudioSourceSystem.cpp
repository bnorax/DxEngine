#include "dxpch.h"
#include "AudioSourceSystem.h"
#include "scene/components/AudioSource.h"
#include "renderer/Renderer.h"
#include "scene/components/Transforms.h"
#include "scene/components/AudioListener.h"
namespace sfs = std::filesystem;
void DxEngine::SceneNS::AudioSourceSystem::LoadSoundFromFS()
{
	using namespace Components;
	auto view = registry.view<AudioSource>();
	for (auto entity : view) {
		AudioSource& source = registry.get<AudioSource>(entity);
		sfs::path pathToSound(source.pathToSound);
		source.sound = std::make_shared<DirectX::SoundEffect>(renderer.soundEngine->audioEngine.get(), pathToSound.wstring().c_str());
	}
}

void DxEngine::SceneNS::AudioSourceSystem::PlaySoundOnce()
{
	using namespace Components;
	if (check) {
		return;
	}
	auto view = registry.view<AudioSource>();
	for (auto entity:view) {
		AudioSource& component = registry.get<AudioSource>(entity);
		Transforms& transform = registry.get<Transforms>(entity);
		component.emitter.SetPosition(transform.position);
		component.emitter.ChannelCount = 2;
		auto viewL = registry.view<AudioListener>();
		for (auto entityL : viewL) {
			AudioListener& list = registry.get<AudioListener>(entityL);
			auto effect = component.sound->CreateInstance(SoundEffectInstance_Use3D);
			effect->Play(true);
			effect->Apply3D(list.listener, component.emitter, false);
			check = true;
		}
	}
	//check = true;
}

void DxEngine::SceneNS::AudioSourceSystem::UpdateFrame()
{
	if (!renderer.soundEngine->audioEngine->Update())
	{
		// No audio device is active
		if (renderer.soundEngine->audioEngine->IsCriticalError())
		{
		}
	}
}
