#include "AudioListenerSystem.h"
#include "scene/components/AudioListener.h"
#include "scene/components/Transforms.h"

void DxEngine::SceneNS::AudioListenerSystem::UpdateFrame()
{
	using namespace Components;
	auto view = registry.view<AudioListener>();
	for (auto entity : view) {
		AudioListener& listener = view.get<AudioListener>(entity);
		Transforms& transform = registry.get<Transforms>(entity);
		listener.listener.SetPosition(transform.position);
	}
}
