#pragma once

namespace DxEngine {
	class EditorCamera {
		POINT mPrev, mCur;
		HWND* currentWindowHandler;
	public:
		void EditorCameraUpdate();

		EditorCamera(HWND* window) {
			currentWindowHandler = window;
		}
	};
}