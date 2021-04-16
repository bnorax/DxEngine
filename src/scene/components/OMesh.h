#pragma once
#include <directXmath.h>
#include "Model.h"
#include "render/InitBuff.h"
#include "core/Time.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace DxEngine {
	class Model;

	struct Vertex {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texCoord;
		DirectX::XMFLOAT3 normal;
		DirectX::XMUINT4 boneIDs = { 0, 0, 0, 0 };
		DirectX::XMFLOAT4 boneWeights = { 0, 0, 0, 0 };
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

	class Mesh {
	public:
		Mesh(); //constructors

		//void MeshInit();
		//void BoneTransform(float timeInSeconds);//methods
		//unsigned int FindPosition(float AnimationTime, const aiNodeAnim * pNodeAnim);
		//unsigned int FindRotation(float AnimationTime, const aiNodeAnim * pNodeAnim);
		//unsigned int FindScaling(float AnimationTime, const aiNodeAnim * pNodeAnim);
		//void CalcInterpolatedPosition(aiVector3D & Out, float AnimationTime, const aiNodeAnim * pNodeAnim);
		//void CalcInterpolatedRotation(aiQuaternion & Out, float AnimationTime, const aiNodeAnim * pNodeAnim);
		//void CalcInterpolatedScaling(aiVector3D & Out, float AnimationTime, const aiNodeAnim * pNodeAnim);
		//void ReadNodeHeirarchy(float AnimationTime, const aiNode * pNode, const DirectX::XMMATRIX& ParentTransform);
		//const aiNodeAnim * FindNodeAnim(const aiAnimation * pAnimation, const std::string NodeName);
		//void Draw(ID3D11DeviceContext *devcon);

		std::vector<Vertex> vertices;//data
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		std::map<std::string, UINT> boneMap; //second parameter = position in vector if bones
		std::vector<Bone> boneList;
		aiScene *scene;
		DirectX::XMMATRIX globalInverseTransform;
		ID3D11Device *device;

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, indexBuffer;
	};
}

//namespace DxEngine {
//	class Model;
//
//	struct Vertex {
//		DirectX::XMFLOAT3 pos;
//		DirectX::XMFLOAT4 color;
//		DirectX::XMFLOAT2 texCoord;
//		DirectX::XMFLOAT3 normal;
//		DirectX::XMUINT4 boneIDs = { 0, 0, 0, 0 };
//		DirectX::XMFLOAT4 boneWeights = { 0, 0, 0, 0 };
//	};
//
//	struct Texture {
//		unsigned int id;
//		std::string type;
//		std::string path;
//		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture = nullptr;
//	};
//
//	struct Bone {
//		DirectX::XMMATRIX offsetMat;
//		DirectX::XMMATRIX finalTransform;
//	};
//
//	struct Animation {
//		std::string a;
//	};
//
//	class Mesh {
//	public:
//		Mesh(); //constructors
//
//		void MeshInit();
//		void BoneTransform(float timeInSeconds);//methods
//		unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
//		unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
//		unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
//		void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
//		void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
//		void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
//		void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const DirectX::XMMATRIX& ParentTransform);
//		const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
//		void Draw(ID3D11DeviceContext* devcon);
//
//		std::vector<Vertex> vertices;//data
//		std::vector<unsigned int> indices;
//		std::vector<Texture> textures;
//		std::map<std::string, UINT> boneMap; //second parameter = position in vector if bones
//		std::vector<Bone> boneList;
//		aiScene* scene;
//		DirectX::XMMATRIX globalInverseTransform;
//		ID3D11Device* device;
//
//	private:
//		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, indexBuffer;
//	};
//}