#include <renderer/Renderer.h>
#include <scene/components/Transforms.h>
#include <core/Time.h>

void DxEngine::SceneNS::CameraSystem::Update(entt::registry& registry)
{
	Time& time = Time::Instance();
	static float rotationX = 0, rotationY = 0;
	POINT curPoint = {0, 0};
	float boost = 1;
	HWND window = GetActiveWindow();
	auto regView = registry.view<Components::Camera>();
	for (auto entity : regView) {
		Components::Transforms& transform = registry.get<Components::Transforms>(entity);
		Components::Camera& camera = registry.get<Components::Camera>(entity);
		GetCursorPos(&curPoint);
		ScreenToClient(window, &curPoint);

		float screenWidth = renderer.windowRef.width, screenHeight = renderer.windowRef.height;
		float distX = curPoint.x - screenWidth / 2;
		float distY = curPoint.y - screenHeight / 2;
		float anglePerPixelX = (camera.maximumY - camera.minimumY) / screenWidth, anglePerPixelY = (camera.maximumX - camera.minimumX) / screenHeight;
		float yaw = anglePerPixelX * distX;
		float yawRadian = yaw * XM_PI / 180.0f;
		float pitch = anglePerPixelY * distY;
		float pitchRadian = pitch * XM_PI / 180.0f;
		XMMATRIX rotation = XMMatrixRotationRollPitchYaw(pitchRadian, yawRadian, 0);
		XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&transform.position));
		float offset = 0.01f;
		float newPosX = offset * sinf(yawRadian) * cosf(pitchRadian);
		float newPosY = offset * -sinf(pitchRadian);
		float newPosZ = offset * cosf(yawRadian) * cosf(pitchRadian);
		if (GetAsyncKeyState(VK_SHIFT) < 0) {
			boost = 0.5;
		}
		if (GetAsyncKeyState(0x57) < 0) {
			transform.position.x += newPosX * boost;
			transform.position.y += newPosY * boost;
			transform.position.z += newPosZ * boost;
		}
		if (GetAsyncKeyState(0x53) < 0) {
			transform.position.x -= newPosX * boost;
			transform.position.y -= newPosY * boost;
			transform.position.z -= newPosZ * boost;
		}
		if (GetAsyncKeyState(0x44) < 0) {
			transform.position.x += newPosZ * boost;
			//transform.position.y -= newPosY * boost;
			transform.position.z -= newPosX * boost;
		}
		if (GetAsyncKeyState(0x41) < 0) {
			transform.position.x -= newPosZ * boost;
			//transform.position.y += newPosY * boost;
			transform.position.z += newPosX * boost;
		}




		//platform specific things
		ConstantBufferPerFrame cb;
		cb.view = XMMatrixIdentity();
		cb.view *= rotation;
		cb.view *= translation;
		cb.view = XMMatrixInverse(nullptr, cb.view);
		cb.view = XMMatrixTranspose(cb.view);
		renderer.deviceContext->UpdateSubresource(renderer.renderStates->cbPerFrame.Get(), 0, nullptr, &cb, 0, 0);
		ShowCursor(false);
	}
	
}

void DxEngine::SceneNS::CameraSystem::UpdateViewMatrix(Components::Transforms& transform)
{
	ConstantBufferPerFrame cb = renderer.cbPerFrame;
	cb.view *= XMMatrixTranslationFromVector(XMLoadFloat3(&transform.position));
	cb.view = XMMatrixTranspose(cb.view);
	renderer.deviceContext->UpdateSubresource(renderer.renderStates->cbPerFrame.Get(), 0, nullptr, &cb, 0, 0);
}