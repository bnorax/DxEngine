#pragma once
#include <dxpch.h>
#include <DirectXMath.h>

namespace DxEngine {
	class EditorCamera {
		POINT prevPoint, curPoint;
		HWND currentWindowHandler;
	public:
		void EditorCameraUpdate();

		EditorCamera(HWND windowHandler) {
			currentWindowHandler = windowHandler;
		}
	};
}