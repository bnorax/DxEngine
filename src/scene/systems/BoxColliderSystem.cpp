#include "BoxColliderSystem.h"
#include "renderer/Renderer.h"
#include "scene/components/BoxCollider.h"
#include "scene/components/Transforms.h"
#include "scene/components/Camera.h"
#include "scene/components/Renderable.h"
#include "renderer/GraphicObjects.h"
#include <DirectXCollision.h>

void DxEngine::SceneNS::BoxColliderSystem::RaycastCollision(entt::registry& registry)
{
	using namespace Components;
	auto regView = registry.view<BoxCollider>();
	for (auto entity : regView) {
		auto collider = regView.get<BoxCollider>(entity);
		auto camera = registry.get<Camera>(entity);
		float x, y;
		x = (2.0f * camera.prevPoint.x) / renderer.windowRef.width - 1.0f;
		y = 1.0f - (2.0f - camera.prevPoint.y) / renderer.windowRef.height;
		//XMFLOAT3 origin(x, y, 0);
		//XMFLOAT3 end(x, y, 1);
		//XMVECTOR rayOrigin = XMLoadFloat3(&origin);
		//XMVECTOR rayEnd = XMLoadFloat3(&end);
		//XMMATRIX inverseProj = XMMatrixInverse(nullptr, renderer.cbPerFrame.view * renderer.cbPerRender.projection);
		//rayOrigin = XMVector3Transform(rayOrigin, inverseProj);
		//rayEnd = XMVector3Transform(rayEnd, inverseProj);
		//XMVECTOR direction = rayEnd - rayOrigin;
		//direction = XMVector3Normalize(direction);
		XMVECTOR start = XMVector3Unproject(XMVECTOR{ x, y,  0}, 0, 0, renderer.windowRef.width, renderer.windowRef.height, 0, 1, renderer.cbPerRender.projection, renderer.cbPerFrame.view, XMMatrixIdentity());
		XMVECTOR end = XMVector3Unproject(XMVECTOR{ x, y,  1 }, 0, 0, renderer.windowRef.width, renderer.windowRef.height, 0, 1, renderer.cbPerRender.projection, renderer.cbPerFrame.view, XMMatrixIdentity());
		XMVECTOR dir = end - start;
		dir = XMVector3Normalize(dir);
		float length = 100.0f;
		if (collider.colliderBox.Intersects(start, dir, length)) {
			auto r = registry.get<Renderable>(entity);
			r.render = false;
			registry.replace<Renderable>(entity, r);
			return;
		}
		auto r = registry.get<Renderable>(entity);
		r.render = true;
		registry.replace<Renderable>(entity, r);
	}
}
