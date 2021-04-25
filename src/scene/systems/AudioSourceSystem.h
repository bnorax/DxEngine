#pragma once
#include <entt/entt.hpp>
namespace DxEngine{
	class Renderer;
	namespace SceneNS {
		class AudioSourceSystem {
		public:
			AudioSourceSystem(Renderer& pRend, entt::registry& pReg) : renderer(pRend), registry(pReg) {};
		private:
			Renderer& renderer;
			entt::registry& registry;
		};
	}
}