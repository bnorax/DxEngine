#pragma once
#include <DirectXMath.h>
#include <array>

using namespace DirectX;
using namespace Microsoft::WRL;


struct position3f {
	float x, y, z;
};
struct direction3f {
	float x, y, z;
};

namespace DxEngine {
	struct Vertex;//forward declaration
	struct Mesh;
	struct Texture;
	struct Bone;
	struct Animation;

	struct Ray {
		position3f pos;
		direction3f direction;
	};

	struct Vertex {
		XMFLOAT3 pos;
		XMFLOAT4 color;
		XMFLOAT2 texCoord;
		XMFLOAT3 normal;
		XMUINT4 boneIDs = { 0, 0, 0, 0 };
		XMFLOAT4 boneWeights = { 0, 0, 0, 0 };
	};
	struct Mesh {
		std::vector<Vertex> vertices;//data
		std::vector<unsigned int> indices;
		std::vector<std::shared_ptr<Texture>> textures;
		std::map<std::string, unsigned int> boneMap; //second parameter = position in vector if bones
		std::vector<Bone> boneList;
		XMMATRIX meshInverseTransform;
		ComPtr<ID3D11Buffer> vertexBuffer, indexBuffer;
	};
	struct Texture {
		unsigned int id;
		std::string type;
		std::string path;
		ComPtr<ID3D11ShaderResourceView> texture = nullptr;
	};
	struct Bone {
		XMMATRIX offsetMat;
		XMMATRIX finalTransform;
	};
	struct Animation {
		std::string name;
		double duration;
		double tickPerSecond;
		int numChannels;
		//std::vector<>
	};
}