#pragma once
#include <array>
#include "renderer/GraphicObjects.h"
namespace DxEngine {
	namespace SceneNS::Components {
		struct Transforms {
			Transforms(std::array<float, 3> pRotation, std::array<float, 3> pScale) :
				rotation(pRotation),
				scale(pScale){};
			position3f pos;
			std::array<float, 3> rotation;
			std::array<float, 3> scale;
		};
	}
}