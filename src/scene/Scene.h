#pragma once
#include <dxpch.h>
#include "entt/entt.hpp"
//#include "scene/Components.h"
//#include "scene/systems/RenderSystem.h"
//#include "scene/systems/CameraSystem.h"

//class Model;

namespace DxEngine {
	class Renderer;
	namespace SceneNS {
		class Scene {
		public:
			//Scene(Renderer& pRenderer) : renderer(pRenderer){};
			Scene() = default;
			std::string name;
			entt::registry registry;
		private:
			//Renderer& renderer;
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