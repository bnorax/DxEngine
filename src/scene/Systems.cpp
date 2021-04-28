#include "dxpch.h"
#include "Systems.h"
#include <scene/Scene.h>

void DxEngine::SceneNS::Systems::InitSystems()
{
	renderSystem = std::make_unique<RenderSystem>(renderer);
	modelSystem = std::make_unique<ModelSystem>(renderer);
	cameraSystem = std::make_unique<CameraSystem>(renderer);
	boxColliderSystem = std::make_unique<BoxColliderSystem>(renderer);
	audioSystem = std::make_unique<AudioSystem>(renderer, scene.registry);
}

void DxEngine::SceneNS::Systems::UpdateFrame()
{
	audioSystem->Update();
	cameraSystem->Update(scene.registry);
	//boxColliderSystem->RaycastCollision(scene.registry);
	renderSystem->Draw(scene.registry);
	modelSystem->Draw(scene.registry);
}
