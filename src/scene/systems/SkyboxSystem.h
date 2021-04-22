#pragma once
#include "entt/entt.hpp"
namespace DxEngine {
	class Renderer;
	namespace SceneNS {
		class SkyboxSystem {
		public:
			SkyboxSystem(Renderer& pRenderer) : renderer(pRenderer) {};
			void Load();
			void Draw(entt::registry&);
		private:
			Renderer& renderer;
		};
	}
}