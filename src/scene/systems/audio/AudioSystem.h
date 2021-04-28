#pragma once
#include <entt/entt.hpp>
#include <Audio.h>
#include "scene/systems/audio/AudioSourceSystem.h"
#include "scene/systems/audio/AudioListenerSystem.h"
namespace DxEngine {
	class Renderer;
	namespace SceneNS {
		class AudioSystem {
		public:
			AudioSystem(Renderer& pRend, entt::registry& pReg);
			void Update();
			std::unique_ptr<DirectX::AudioEngine> audioEngine;
			std::unique_ptr<AudioSourceSystem> sourceSystem;
			std::unique_ptr<AudioListenerSystem> listenerSystem;
		private:
			Renderer& renderer;
			entt::registry& registry;
		};
	}
}