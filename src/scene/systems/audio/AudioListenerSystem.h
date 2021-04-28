#pragma once
#include <entt/entt.hpp>
namespace DirectX {
	class AudioEngine;
}
namespace DxEngine::SceneNS {
	class AudioListenerSystem {
	public:
		AudioListenerSystem(DirectX::AudioEngine*, entt::registry&);
		void Update();
	private:
		DirectX::AudioEngine* audioEngine;
		entt::registry& registry;
	};
}