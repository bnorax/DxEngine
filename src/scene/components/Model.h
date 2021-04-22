#pragma once
#include "renderer/GraphicObjects.h"

namespace DxEngine{
	namespace SceneNS::Components {
		struct Model {
			Model(std::string pFilePath) : filePath(pFilePath) {};
			DirectX::XMMATRIX worldTransform;
			DirectX::XMMATRIX modelInverseTransform;
			std::vector<std::shared_ptr<Mesh>> meshes;
			std::vector<std::shared_ptr<Texture>> loadedTextures;
			std::string filePath;
		};
	}
}