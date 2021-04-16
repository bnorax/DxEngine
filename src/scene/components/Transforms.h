#pragma once
#include <dxpch.h>

namespace DxEngine {
	namespace SceneNS::Components {
		struct Transforms {
			Transforms(std::array<float, 3> pPosition, std::array<float, 3> pRotation, std::array<float, 3> pScale) :
				position(pPosition),
				rotation(pRotation),
				scale(pScale){};
			std::array<float, 3> position;
			std::array<float, 3> rotation;
			std::array<float, 3> scale;
		};
	}
}