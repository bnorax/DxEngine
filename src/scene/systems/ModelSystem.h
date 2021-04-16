#pragma once
#include "entt/entt.hpp"
#include <assimp/scene.h>
#include "renderer/Renderer.h"
#include <scene/components/Model.h>
#include <scene/components/Skybox.h>

namespace DxEngine {
	class Renderer;
	namespace SceneNS {
		class ModelSystem {
		public:
			ModelSystem(Renderer& pRenderer) : renderer(pRenderer) {};

			bool Load(entt::registry&);
			void Draw(entt::registry&);

			void BoneTransform(float timeInSeconds);//methods
			//void Draw(ID3D11DeviceContext* devcon);

		private:
			Renderer& renderer;
			void MeshInit(Mesh&);

			void processNode(aiNode*, const aiScene*, Components::Model&);
			std::shared_ptr<Mesh> processMesh(Components::Model&, aiMesh*, const aiScene*);
			std::vector<std::shared_ptr<Texture>> loadMaterialTextures(Components::Model&, aiMaterial*, aiTextureType, std::string, const aiScene*);
			int getTextureIndex(aiString*);

			const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
			unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
			unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
			unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
			void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
			void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
			void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
			void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const DirectX::XMMATRIX& ParentTransform);
		};
	}
}

DirectX::XMMATRIX aiToXMMATRIX(const aiMatrix4x4&);
DirectX::XMMATRIX aiToXMMATRIX(const aiMatrix3x3&);