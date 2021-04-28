#pragma once
#include <entt/entt.hpp>
namespace DirectX {
	class AudioEngine;
}
namespace DxEngine{
	class Renderer;
	namespace SceneNS {
		class AudioSourceSystem {
		public:
			AudioSourceSystem(DirectX::AudioEngine*, entt::registry&);
			void LoadSoundFromFS();
			void PlaySoundOnce();
			void Update();
		private:
			DirectX::AudioEngine* audioEngine;
			entt::registry& registry;
		};
	}
}