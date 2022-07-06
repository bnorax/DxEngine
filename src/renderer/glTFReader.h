#pragma once
#include "scene/components/Model.h"
#include "tinygltf/tiny_gltf.h"

namespace DxEngine {
	class ModelLoader {
		tinygltf::TinyGLTF loader;
		//DirectX::XMMATRIX TransformToXMMATRIX(std::vector<double>&);
		//void ProccessNode(tinygltf::Node&, SceneNS::Components::Model&);
	public:
		ModelLoader();

		SceneNS::Components::Model LoadFile(std::string filePath);

	};

}