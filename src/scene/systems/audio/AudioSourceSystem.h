#pragma once
#include <entt/entt.hpp>

namespace DxEngine{
	class Renderer;
	namespace SceneNS {
		class AudioSourceSystem {
		public:
			AudioSourceSystem(Renderer& pRend, entt::registry& pReg) : renderer(pRend), registry(pReg) {};
			void LoadSoundFromFS();
			void PlaySoundOnce();
			void UpdateFrame();
			bool check = false;
		private:
			Renderer& renderer;
			entt::registry& registry;
		};
	}
}