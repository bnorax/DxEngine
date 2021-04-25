#pragma once
#include <dxpch.h>
#include "scene/systems/ModelSystem.h"
#include "scene/systems/RenderSystem.h"
#include "scene/systems/CameraSystem.h"
#include "scene/systems/BoxColliderSystem.h"
#include "scene/systems/audio/AudioSystem.h"

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
			std::unique_ptr<BoxColliderSystem> boxColliderSystem;
			std::unique_ptr<AudioSystem> audioSystem;
			void UpdateFrame();
		private:
			Renderer& renderer;
			Scene& scene;
		};
	}
}