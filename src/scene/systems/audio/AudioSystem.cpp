#include "AudioSystem.h"
using namespace DirectX;
DxEngine::SceneNS::AudioSystem::AudioSystem(Renderer& pRend, entt::registry& pReg)
	: renderer(pRend), registry(pReg)
{
	sourceSystem = std::make_unique<AudioSourceSystem>(pRend, pReg);
	listenerSystem = std::make_unique<AudioListenerSystem>(pRend, pReg);
	AUDIO_ENGINE_FLAGS aeflags = AudioEngine_Default;

#ifdef _DEBUG
	aeflags |= AudioEngine_Debug;
#endif
	audioEngine = std::make_unique<AudioEngine>(aeflags);
}

void DxEngine::SceneNS::AudioSystem::UpdateFrame()
{
	audioEngine->Update();
}
