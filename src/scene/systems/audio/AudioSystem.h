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
			std::unique_ptr<AudioSourceSystem> sourceSystem;
			std::unique_ptr<AudioListenerSystem> listenerSystem;
			std::unique_ptr<DirectX::AudioEngine> audioEngine;
			void UpdateFrame();
		private:
			Renderer& renderer;
			entt::registry& registry;
		};
	}
}