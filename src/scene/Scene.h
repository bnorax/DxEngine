#pragma once
#include <dxpch.h>
#include "DxEngine.h"

class Model;

namespace DxEngine {
	namespace SceneNS {
		class Scene {
		public:
			Scene() {};
			std::string name;
			//std::vector<Model> models;
			/*std::vector<Camera> cameras;
			std::vector<LightSource> lightSources;*/
		};
		class SceneLoader {
		public:
			SceneLoader(std::shared_ptr<Scene> pScene) : scene(pScene) {};
			SceneLoader() {};
			std::shared_ptr<Scene> loadFromFile(std::wstring path);
			bool saveToFile(std::shared_ptr<Scene> scene);
		private:
			std::shared_ptr<Scene> scene;
		};
	}
}