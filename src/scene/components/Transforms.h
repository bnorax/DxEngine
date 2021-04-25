#pragma once
#include <array>
#include <DirectXMath.h>
namespace DxEngine {
	namespace SceneNS::Components {
		struct Transforms {
			Transforms(DirectX::XMFLOAT3 pPosition, DirectX::XMFLOAT3 pRotation, DirectX::XMFLOAT3 pScale) :
				position(pPosition),
				rotation(pRotation),
				scale(pScale){};
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 rotation;
			DirectX::XMFLOAT3 scale;
		};
	}
}