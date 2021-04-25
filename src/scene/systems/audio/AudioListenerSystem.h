#pragma once
#include <entt/entt.hpp>

namespace DxEngine {
	class Renderer;
	namespace SceneNS {
		class AudioListenerSystem {
		public:
			AudioListenerSystem(Renderer& pRend, entt::registry& pReg) : renderer(pRend), registry(pReg) {};
			void UpdateFrame();
		private:
			Renderer& renderer;
			entt::registry& registry;
		};
	}
}