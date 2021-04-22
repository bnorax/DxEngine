#pragma once
#include <dxpch.h>
#include "entt/entt.hpp"

namespace DxEngine {
	class Renderer;
	namespace SceneNS {
		class Scene {
		public:
			Scene() = default;
			std::string name;
			entt::registry registry;
		};

		class SceneLoader {
		public:
			SceneLoader(std::shared_ptr<Scene> pScene) : scene(pScene) {};
			SceneLoader() {};
			std::shared_ptr<Scene> loadFromFile(std::wstring path);
			bool SaveToFile(std::shared_ptr<Scene> scene);
		private:
			std::shared_ptr<Scene> scene;
		};
	}
}