#include <dxpch.h>
#include "ModelLoader.h"
#include "InitBuff.h"

ModelLoader::ModelLoader()
{
}

ModelLoader::~ModelLoader()
{
	scene->~aiScene();
}

bool ModelLoader::Load(HWND hwnd, ID3D11Device * device, ID3D11DeviceContext * devCon, std::string filename)
{
	Assimp::Importer importer;
	importer.ReadFile(filename,
		aiProcess_Triangulate |
		aiProcess_RemoveRedundantMaterials | // remove redundant materials
		aiProcess_FindDegenerates | // remove degenerated polygons from the import
		aiProcess_FindInvalidData |
		aiProcess_SortByPType  |
		aiProcess_FlipUVs |
		aiProcess_ConvertToLeftHanded);
	aiScene* sceneI = importer.GetOrphanedScene();
	if (sceneI == nullptr) return false;
	this->scene = sceneI;
	this->filepath = filename.substr(0, filename.find_last_of("/\\"));
	this->device = device;
	this->devCon = devCon;
	this->hwnd = hwnd;
	processNode(scene->mRootNode, scene);

	return true;
}

void ModelLoader::Draw(ID3D11DeviceContext * devCon)
{
	for (size_t i = 0; i < meshes.size(); ++i) {
		meshes[i].Draw(devCon);
	}
}

std::string texType;

DirectX::XMMATRIX aiToXMMATRIX(const aiMatrix4x4& AssimpMatrix) //делаем из aiMatrix(assimp) -> матрицу из директа(XMfloat4x4)
{
	DirectX::XMMATRIX m;
	m = DirectX::XMMatrixIdentity();
	m = DirectX::XMMatrixSet(
	AssimpMatrix.a1, AssimpMatrix.a2, AssimpMatrix.a3, AssimpMatrix.a4,
	AssimpMatrix.b1, AssimpMatrix.b2, AssimpMatrix.b3, AssimpMatrix.b4,
	AssimpMatrix.c1, AssimpMatrix.c2, AssimpMatrix.c3, AssimpMatrix.c4,
	AssimpMatrix.d1, AssimpMatrix.d2, AssimpMatrix.d3, AssimpMatrix.d4);
	return m;
}

DirectX::XMMATRIX aiToXMMATRIX(const aiMatrix3x3& AssimpMatrix) //делаем из aiMatrix(assimp) -> матрицу из директа(XMfloat4x4)
{
	DirectX::XMMATRIX m;
	m = DirectX::XMMatrixIdentity();
	m = DirectX::XMMatrixSet(
		AssimpMatrix.a1, AssimpMatrix.a2, AssimpMatrix.a3, 0,
		AssimpMatrix.b1, AssimpMatrix.b2, AssimpMatrix.b3, 0,
		AssimpMatrix.c1, AssimpMatrix.c2, AssimpMatrix.c3, 0,
		0,0, 0, 1);
	return m;
}

void AddBoneData(SimpleVertex &vert, UINT boneID, float weight) {
		if (vert.boneWeights.x == 0.0) {
			vert.boneIDs.x = boneID;
			vert.boneWeights.x = weight;
			return;
		}
		if (vert.boneWeights.y == 0.0) {
			vert.boneIDs.y = boneID;
			vert.boneWeights.y = weight;
			return;
		}
		if (vert.boneWeights.z == 0.0) {
			vert.boneIDs.z = boneID;
			vert.boneWeights.z = weight;
			return;
		}
		if (vert.boneWeights.w == 0.0) {
			vert.boneIDs.w = boneID;
			vert.boneWeights.w = weight;
			return;
		}
}



Mesh ModelLoader::processMesh(aiMesh * mesh, aiScene * scene)
{
	Mesh *curMeshP;
	curMeshP = new Mesh();
	Mesh &curMesh = *curMeshP;
	//std::vector<SimpleVertex> vertices;
	//std::vector<UINT> indices;
	//std::vector<Texture> textures;
	//std::map<std::string, UINT> boneMap;
	//std::vector<Bone> boneList;

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

		if (texType.empty()) {
			texType = determineTextureType(scene, mat);
		}
	}

	for (UINT i = 0; i < mesh->mNumVertices; i++) {
		SimpleVertex vertex;

		vertex.pos.x = mesh->mVertices[i].x;
		vertex.pos.y = mesh->mVertices[i].y;
		vertex.pos.z = mesh->mVertices[i].z;
		vertex.color.x = 1;
		vertex.color.y = 0;
		vertex.color.z = 0;
		vertex.color.w = 1;

		if (mesh->mTextureCoords[0]) {
			vertex.texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		curMesh.vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			curMesh.indices.push_back(face.mIndices[j]);
	}


	if (mesh->HasBones()) {
		this->numBones = 0;
		for (UINT i = 0; i < mesh->mNumBones; i++) {
			UINT boneIndex = 0;
			std::string boneName(mesh->mBones[i]->mName.data);
			if (curMesh.boneMap.find(boneName) == curMesh.boneMap.end()) {
				boneIndex = numBones;
				numBones++;
				Bone bone;
				//bone.offsetMat = aiToXMMATRIX(mesh->mBones[i]->mOffsetMatrix);
				curMesh.boneList.push_back(bone);
				//curMesh.boneMap.insert(std::make_pair(boneName, boneIndex));
			}
			else{
				boneIndex = curMesh.boneMap[boneName];
			}
			curMesh.boneMap[boneName] = boneIndex;
			curMesh.boneList[boneIndex].offsetMat = aiToXMMATRIX(mesh->mBones[i]->mOffsetMatrix);

			for (UINT j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
				UINT vertID = mesh->mBones[i]->mWeights[j].mVertexId;
				float weight = mesh->mBones[i]->mWeights[j].mWeight;
				AddBoneData(curMesh.vertices[vertID], boneIndex, weight);
			}
		}
	}
	 
	
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
		curMesh.textures.insert(curMesh.textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	}

	return Mesh(scene, device, curMesh.vertices, curMesh.indices, curMesh.textures, curMesh.boneMap, curMesh.boneList);
}

void ModelLoader::processNode(aiNode * node, aiScene * scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh meshA = this->processMesh(mesh, scene);
		meshA.meshInitTransform = aiToXMMATRIX(scene->mRootNode->mTransformation);
		meshes.push_back(meshA);
	}

	for (UINT i = 0; i < node->mNumChildren; i++) {
		this->processNode(node->mChildren[i], scene);
	}
}


std::vector<Texture> ModelLoader::loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName,  aiScene * scene)
{
	std::vector<Texture> textures;
	for (UINT i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (UINT j = 0; j < loadedTextures.size(); j++) {
			if (std::strcmp(loadedTextures[j].path.c_str(), str.C_Str()) == 0) {
				textures.push_back(loadedTextures[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip) {   // If texture hasn't been loaded already, load it
			HRESULT hr;
			Texture texture;
			if (texType == "embedded compressed texture") {
				int textureindex = getTextureIndex(&str);
				//ComPtr<ID3D11ShaderResourceView> tex = getTextureFromModel(scene, textureindex);
				getTextureFromModel(scene, texture.texture.GetAddressOf(), textureindex);
				//hr = DirectX::CreateWICTextureFromMemory(device, devCon, sizeof(tex), );
				//hr = DirectX::CreateWICTextureFromFile(device, devCon, filenamews.c_str(), nullptr, texture.texture.GetAddressOf());
				
			}
			else {
				std::string filename = std::string(str.C_Str());
				filename = filepath + '/' + filename;
				std::wstring filenamews = std::wstring(filename.begin(), filename.end());
				hr = DirectX::CreateWICTextureFromFile(device, devCon, filenamews.c_str(), nullptr, texture.texture.GetAddressOf());
				if (FAILED(hr))
					MessageBox(hwnd, L"Texture couldn't be loaded", L"Error!", MB_ICONERROR | MB_OK);
			}
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			this->loadedTextures.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

std::string ModelLoader::determineTextureType(const aiScene * scene, aiMaterial * mat)
{
	aiString textypeStr;
	mat->GetTexture(aiTextureType_DIFFUSE, 0, &textypeStr);
	std::string textypeteststr = textypeStr.C_Str();
	if (textypeteststr == "*0" || textypeteststr == "*1" || textypeteststr == "*2" || textypeteststr == "*3" || textypeteststr == "*4" || textypeteststr == "*5") {
		if (scene->mTextures[0]->mHeight == 0) {
			return "embedded compressed texture";
		}
		else {
			return "embedded non-compressed texture";
		}
	}
	if (textypeteststr.find('.') != std::string::npos) {
		return "textures are on disk";
	}

	return ".";
}

int ModelLoader::getTextureIndex(aiString * str)
{
	std::string tistr;
	tistr = str->C_Str();
	tistr = tistr.substr(1);
	return stoi(tistr);
}

void ModelLoader::getTextureFromModel(const aiScene * scene, ID3D11ShaderResourceView **texView, int textureIndex)
{
	HRESULT hr;
	hr = DirectX::CreateWICTextureFromMemory(device, (const uint8_t*)(scene->mTextures[textureIndex]->pcData), scene->mTextures[textureIndex]->mWidth, nullptr, texView);
	//int* size = reinterpret_cast<int*>(&scene->mTextures[textureIndex]->mWidth);
	//hr = DirectX::CreateWICTextureFromMemory(device, devCon, reinterpret_cast<unsigned char*>(scene->mTextures[textureIndex]->pcData), *size, nullptr, texView);
	if (FAILED(hr))
		MessageBox(hwnd, L"Texture couldn't be created from memory!", L"Error!", MB_ICONERROR | MB_OK);
}
