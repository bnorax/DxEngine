#pragma once
#include <entt/entt.hpp>
class Renderer;
namespace DxEngine::SceneNS {
	namespace Components {
		struct Transforms;
	}
	class CameraSystem {
	public:
		CameraSystem(Renderer& pRenderer) : renderer(pRenderer){};
		void Update(entt::registry&);
	private:
		void UpdateViewMatrix(Components::Transforms&);
		Renderer& renderer;
	};
}