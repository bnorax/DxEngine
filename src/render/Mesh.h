#pragma once
#include <directXmath.h>
#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "core/Time.h"


class Model;

	struct SimpleVertex {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texCoord;
		DirectX::XMFLOAT3 normal;
		DirectX::XMUINT4 boneIDs = {0, 0, 0, 0};
		DirectX::XMFLOAT4 boneWeights = { 0, 0, 0, 0 };
		//std::map<UINT, float> boneInfo; //<boneID, boneWeight>
	};

	struct Texture {
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
		Mesh(aiScene *ascene, DirectX::XMMATRIX glTransform, ID3D11Device *dev, const std::vector<SimpleVertex> vert, const std::vector<UINT> ind, const std::vector<Texture>& tex, const std::map<std::string, UINT> boneM, const std::vector<Bone> boneL);


		void MeshInit();
		void BoneTransform(float timeInSeconds);//methods
		UINT FindPosition(float AnimationTime, const aiNodeAnim * pNodeAnim);
		UINT FindRotation(float AnimationTime, const aiNodeAnim * pNodeAnim);
		UINT FindScaling(float AnimationTime, const aiNodeAnim * pNodeAnim);
		void CalcInterpolatedPosition(aiVector3D & Out, float AnimationTime, const aiNodeAnim * pNodeAnim);
		void CalcInterpolatedRotation(aiQuaternion & Out, float AnimationTime, const aiNodeAnim * pNodeAnim);
		void CalcInterpolatedScaling(aiVector3D & Out, float AnimationTime, const aiNodeAnim * pNodeAnim);
		void ReadNodeHeirarchy(float AnimationTime, const aiNode * pNode, const DirectX::XMMATRIX& ParentTransform);
		const aiNodeAnim * FindNodeAnim(const aiAnimation * pAnimation, const std::string NodeName);
		void Draw(ID3D11DeviceContext *devcon);

		std::vector<SimpleVertex> vertices;//data
		std::vector<UINT> indices;
		std::vector<Texture> textures;
		std::map<std::string, UINT> boneMap; //second parameter = position in vector if bones
		std::vector<Bone> boneList;
		aiScene *scene;;
		DirectX::XMMATRIX globalInverseTransform;
		ID3D11Device *device;
		
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, indexBuffer;
	};