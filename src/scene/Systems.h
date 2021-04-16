#pragma once
#include "scene/systems/ModelSystem.h"
#include "scene/systems/RenderSystem.h"
#include "scene/systems/CameraSystem.h"

namespace DxEngine {
	class Renderer;
	namespace SceneNS {
		class Scene;
		namespace SystemsNS {
			class RenderSystem;
			class ModelSystem;
		}
		class Systems {
		public:
			Systems(Renderer& pRenderer, Scene& pScene) : renderer(pRenderer), scene(pScene) {};
			void InitSystems();
			std::unique_ptr<RenderSystem> renderSystem;
			std::unique_ptr<ModelSystem> modelSystem;
			std::unique_ptr<CameraSystem> cameraSystem;
			void UpdateFrame();
		private:
			Renderer& renderer;
			Scene& scene;
		};
	}
}