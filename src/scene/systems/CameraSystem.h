#pragma once
#include <entt/entt.hpp>
class Renderer;
namespace DxEngine::SceneNS {
	class CameraSystem {
	public:
		CameraSystem(Renderer& pRenderer) : renderer(pRenderer){};
		void Update(entt::registry&);
	private:
		Renderer& renderer;
	};
}