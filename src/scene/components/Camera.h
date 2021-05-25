#pragma once
namespace DxEngine::SceneNS::Components {
	struct Camera {
	public:
		POINT prevPoint;

		float sensitivityX = 2.0f;
		float sensitivityY = .005f;

		float minimumY = 0.0f;
		float maximumY = 360.0f;

		float minimumX = -10.0f;
		float maximumX = 20.0f;
		//float boost = 1;
	};
}