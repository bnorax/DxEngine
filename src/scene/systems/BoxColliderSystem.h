#pragma once
#include <entt/entt.hpp>
namespace DxEngine {
	class Renderer;
}
namespace DxEngine::SceneNS {
	class BoxColliderSystem {
	public:
		BoxColliderSystem(Renderer& pRenderer) : renderer(pRenderer) {};
		void RaycastCollision(entt::registry&);
	private:
		Renderer& renderer;
	};
}