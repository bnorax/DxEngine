#pragma once
#include <dxpch.h>
#include <DxEngine.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <WICTextureLoader.h>

class ModelLoader {
public:
	ModelLoader();
	~ModelLoader();

	bool Load(HWND hwnd, ID3D11Device *device, ID3D11DeviceContext *devCon, std::string filename);
	void Draw(ID3D11DeviceContext *devCon);
private:
	ID3D11Device *device;
	ID3D11DeviceContext *devCon;
	HWND hwnd;
	std::vector<Mesh> meshes;
	std::vector<Texture> loadedTextures;
	std::string filepath;

	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene);
	std::string determineTextureType(const aiScene* scene, aiMaterial* mat);
	int getTextureIndex(aiString* str);
	void getTextureFromModel(const aiScene * scene, ID3D11ShaderResourceView **texView, int textureIndex);
};
