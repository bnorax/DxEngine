#include <renderer/Renderer.h>
#include <scene/components/Transforms.h>

void DxEngine::SceneNS::CameraSystem::Update(entt::registry& registry)
{
	auto regView = registry.view<Components::Camera>();
	for (auto entity : regView) {
		Components::Transforms& transfrom = registry.get<Components::Transforms>(entity);
		Components::Camera& camera = registry.get<Components::Camera>(entity);
		float boost = 1;
		if (GetAsyncKeyState(VK_RBUTTON) < 0) {
			GetCursorPos(&camera.curPoint);
			ScreenToClient(GetActiveWindow(), &camera.curPoint);
			if (&camera.curPoint || &camera.prevPoint != NULL) {
				if (camera.curPoint.y - camera.prevPoint.y > 1) {
					renderer.cbPerFrame.view *= XMMatrixRotationX(-0.01f);
				}
				else if (camera.curPoint.y - camera.prevPoint.y < -1) {
					renderer.cbPerFrame.view *= XMMatrixRotationX(0.01f);
				}
				if (camera.curPoint.x - camera.prevPoint.x > 1) {
					renderer.cbPerFrame.view *= XMMatrixRotationY(-0.02f);
				}
				else if (camera.curPoint.x - camera.prevPoint.x < -1) {
					renderer.cbPerFrame.view *= XMMatrixRotationY(0.02f);
				}
			}
		}
		if (GetAsyncKeyState(VK_SHIFT) < 0) {
			boost = 10;
		}
		if (GetAsyncKeyState(0x57) < 0) {
			renderer.cbPerFrame.view *= XMMatrixTranslation(0, 0, -0.01f * boost);
			transfrom.position.z -= 0.01f * boost;
		}
		if (GetAsyncKeyState(0x53) < 0) {
			renderer.cbPerFrame.view *= XMMatrixTranslation(0, 0, 0.01f * boost);
			transfrom.position.z += 0.01f * boost;
		}
		if (GetAsyncKeyState(0x44) < 0) {
			renderer.cbPerFrame.view *= XMMatrixTranslation(-0.01f * boost, 0, 0);
			transfrom.position.x -= 0.01f * boost;
		}
		if (GetAsyncKeyState(0x41) < 0) {
			renderer.cbPerFrame.view *= XMMatrixTranslation(0.01f * boost, 0, 0);
			transfrom.position.x += 0.01f * boost;
		}
		GetCursorPos(&camera.prevPoint);
		ScreenToClient(GetActiveWindow(), &camera.prevPoint);
	//	registry.replace<Components::Camera>(entity, camera);
		ConstantBufferPerFrame cb = renderer.cbPerFrame;
		cb.view = XMMatrixTranspose(cb.view);
		renderer.deviceContext->UpdateSubresource(renderer.renderStates->cbPerFrame.Get(), 0, nullptr, &cb, 0, 0);
	}
	
}
