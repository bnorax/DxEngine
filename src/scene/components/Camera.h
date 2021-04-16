#pragma once
namespace DxEngine::SceneNS::Components {
	struct Camera {
	public:
		POINT prevPoint, curPoint;
		//float boost = 1;
	};
}