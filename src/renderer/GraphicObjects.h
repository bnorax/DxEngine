#pragma once
#include <DirectXMath.h>
namespace DxEngine {
	struct Vertex;//forward declaration
	struct Mesh;
	struct Texture;
	struct Bone;
	struct Animation;

	struct Vertex {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texCoord;
		DirectX::XMFLOAT3 normal;
		DirectX::XMUINT4 boneIDs = { 0, 0, 0, 0 };
		DirectX::XMFLOAT4 boneWeights = { 0, 0, 0, 0 };
	};
	struct Mesh {
		std::vector<Vertex> vertices;//data
		std::vector<unsigned int> indices;
		std::vector<std::shared_ptr<Texture>> textures;
		std::map<std::string, unsigned int> boneMap; //second parameter = position in vector if bones
		std::vector<Bone> boneList;
		DirectX::XMMATRIX meshInverseTransform;
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, indexBuffer;
	};
	struct Texture {
		unsigned int id;
		std::string type;
		std::string path;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture = nullptr;
	};
	struct Bone {
		DirectX::XMMATRIX offsetMat;
		DirectX::XMMATRIX finalTransform;
	};
	struct Animation {
		std::string a;
	};
}