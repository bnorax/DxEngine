#include "dxpch.h"
#include "AudioSourceSystem.h"
#include "scene/components/AudioSource.h"
#include "renderer/Renderer.h"
#include "scene/components/Transforms.h"
#include "core/Time.h"
namespace sfs = std::filesystem;
DxEngine::SceneNS::AudioSourceSystem::AudioSourceSystem(DirectX::AudioEngine* pAudio, entt::registry& pReg)
	: audioEngine(pAudio), registry(pReg)
{
	auto view = registry.view<Components::AudioSource>();
	for (auto entity : view) {
		Components::AudioSource& source = registry.get<Components::AudioSource>(entity);
		source.emitter = std::make_unique<DirectX::AudioEmitter>();
		source.emitter->ChannelCount = 2;
	}
}
void DxEngine::SceneNS::AudioSourceSystem::LoadSoundFromFS()
{
	using namespace Components;
	auto view = registry.view<AudioSource>();
	for (auto entity : view) {
		AudioSource& source = registry.get<AudioSource>(entity);
		sfs::path pathToSound(source.pathToSound);
		source.sound = std::make_shared<DirectX::SoundEffect>(audioEngine, pathToSound.wstring().c_str());
	}
}

void DxEngine::SceneNS::AudioSourceSystem::PlaySoundOnce()
{
	using namespace Components;
	auto view = registry.view<AudioSource>();
	for (auto entity:view) {
		AudioSource& component = registry.get<AudioSource>(entity);
		component.soundInstance = component.sound->CreateInstance(SoundEffectInstance_Use3D);
		component.soundInstance->SetVolume(0.05f);
		component.soundInstance->Play();
	}
}

void DxEngine::SceneNS::AudioSourceSystem::PlayLoopedSound()
{
	using namespace Components;
	auto view = registry.view<AudioSource>();
	for (auto entity : view) {
		AudioSource& component = registry.get<AudioSource>(entity);
		component.soundInstance = component.sound->CreateInstance(SoundEffectInstance_Use3D);
		component.soundInstance->SetVolume(0.05f);
		component.soundInstance->Play(true);
	}
}



void DxEngine::SceneNS::AudioSourceSystem::Update()
{
	using namespace Components;
	auto view = registry.view<AudioSource>();
	for (auto entity : view) {
		AudioSource& source = registry.get<AudioSource>(entity);
		Transforms& transform = registry.get<Transforms>(entity);
		source.emitter->SetPosition(transform.position);
	}
}
