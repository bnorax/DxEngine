#include "glTFReader.h"
#include <string>
#include "GraphicObjects.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT

#include "tinygltf/tiny_gltf.h"
//#include "tinygltf/json.hpp"

DirectX::XMMATRIX TransformToXMMATRIX(std::vector<double>& vec) {
	DirectX::XMMATRIX m;
	m = DirectX::XMMatrixIdentity();
	m = DirectX::XMMatrixSet(
		vec[0], vec[1], vec[2], vec[3],
		vec[4], vec[5], vec[6], vec[7],
		vec[8], vec[9], vec[10], vec[11],
		vec[12], vec[13], vec[14], vec[15]);
	//m = DirectX::XMMatrixTranspose(m);
	return m;
};

void ProccessNode(tinygltf::Node& node,tinygltf::Model& loadedModel, DxEngine::SceneNS::Components::Model& model)
{
	DxEngine::Mesh mesh;
	//mesh.meshInverseTransform = TransformToXMMATRIX(node.matrix);

	for (int i = 0; i < loadedModel.meshes[node.mesh].primitives.size(); i++) {
		tinygltf::Primitive primitive = loadedModel.meshes[node.mesh].primitives[i];
		const tinygltf::Accessor& accessor = loadedModel.accessors[primitive.attributes["POSITION"]];
		const tinygltf::BufferView& bufferView = loadedModel.bufferViews[accessor.bufferView];
		const tinygltf::Buffer& buffer = loadedModel.buffers[bufferView.buffer];
		const float* positions = reinterpret_cast<const float*>(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);

		DxEngine::Vertex vertex;
		for (int i = 0; i < accessor.count; ++i) {
			vertex.pos.x = positions[i * 3 + 0];
			vertex.pos.y = positions[i * 3 + 1];
			vertex.pos.z = positions[i * 3 + 2];
		}
		mesh.vertices.push_back(vertex);

	}

	//ProcessMeshData(mesh);

}

//void ProcessMeshData(tinygltf::Node& node, DxEngine::Mesh& mesh) {
//
//	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
//		Vertex vertex;
//		vertex.pos.x = mesh->mVertices[i].x;
//		vertex.pos.y = mesh->mVertices[i].y;
//		vertex.pos.z = mesh->mVertices[i].z;
//		vertex.color.x = 1;
//		vertex.color.y = 0;
//		vertex.color.z = 0;
//		vertex.color.w = 1;
//		if (mesh->mTextureCoords[0]) {
//			vertex.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
//			vertex.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
//		}
//
//		curMesh->vertices.push_back(vertex);
//	}
//	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
//		aiFace face = mesh->mFaces[i];
//
//		for (unsigned int j = 0; j < face.mNumIndices; j++)
//			curMesh->indices.push_back(face.mIndices[j]);
//	}
//}

DxEngine::ModelLoader::ModelLoader()
{
}

DxEngine::SceneNS::Components::Model DxEngine::ModelLoader::LoadFile(std::string filePath)
{
	std::string loaderError;
	std::string loaderWarn;

	tinygltf::Model loadedModel;
	SceneNS::Components::Model engineModel(filePath);
	loader.LoadASCIIFromFile(&loadedModel, &loaderError, &loaderWarn, "resources\\mesh\\avocado\\Avocado.gltf");

	ProccessNode(loadedModel.nodes[0], loadedModel, engineModel);
	return engineModel;
}
