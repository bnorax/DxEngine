#pragma once
namespace DxEngine::SceneNS::Components {
	struct Renderable {
		bool render = true;
		Renderable() { };
		Renderable(bool var) {
			render = var;
		}
	};
}
