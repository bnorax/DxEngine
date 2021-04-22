#pragma once
#include <entt/entt.hpp>
namespace DxEngine {
	class Renderer;
	namespace SceneNS {
		class RenderSystem {
		public:
			RenderSystem(Renderer& pRenderer) : renderer(pRenderer) {};
			void Draw(entt::registry&);
		private:
			Renderer& renderer;
		};
	}
}