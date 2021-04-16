#pragma once
#include <dxpch.h>

#include "scene/components/Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <WICTextureLoader.h>
#include <directXmath.h>


namespace DxEngine::Scene::Components {
	class Mesh;
	struct Texture;
	class Model {
	public:
		Model();
		~Model();

		bool load(HWND, ID3D11Device*, ID3D11DeviceContext*, std::string);
		void draw(ID3D11DeviceContext*);

		DirectX::XMMATRIX worldTransform;
		aiScene *scene;
		DirectX::XMMATRIX globalInverseTransform;
		unsigned int numBones = 0;
	
	private:
		ID3D11Device *device;
		ID3D11DeviceContext *devCon;
		HWND hwnd;
		std::vector<Mesh> meshes;
		std::vector<Texture> loadedTextures;
		std::string filePath;
	
	

		void processNode(aiNode*, aiScene*);
		Mesh processMesh(aiMesh*, aiScene*);
		std::vector<Texture> loadMaterialTextures(aiMaterial*, aiTextureType, std::string, aiScene*);
		int getTextureIndex(aiString*);
	
	
	};
	DirectX::XMMATRIX aiToXMMATRIX(const aiMatrix4x4&);
	DirectX::XMMATRIX aiToXMMATRIX(const aiMatrix3x3&);
}