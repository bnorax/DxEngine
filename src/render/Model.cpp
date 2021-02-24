#include <dxpch.h>
#include "Model.h"

Model::Model()
{
}

Model::~Model()
{
	scene->~aiScene();
}

bool Model::load(HWND pHwnd, ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, std::string filename)
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
	globalInverseTransform = aiToXMMATRIX(sceneI->mRootNode->mTransformation);
	scene = sceneI;
	filePath = filename.substr(0, filename.find_last_of("/\\"));
	device = pDevice;
	devCon = pDeviceContext;
	hwnd = pHwnd;

	processNode(scene->mRootNode, scene);

	return true;
}

void Model::draw(ID3D11DeviceContext * devCon)
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
	m = DirectX::XMMatrixTranspose(m);
	return m;
}

DirectX::XMMATRIX aiToXMMATRIX(const aiMatrix3x3& AssimpMatrix) //делаем из aiMatrix(assimp) -> матрицу из директа(XMfloat3x3)
{
	DirectX::XMMATRIX m;
	m = DirectX::XMMatrixIdentity();
	m = DirectX::XMMatrixSet(
		AssimpMatrix.a1, AssimpMatrix.a2, AssimpMatrix.a3, 0,
		AssimpMatrix.b1, AssimpMatrix.b2, AssimpMatrix.b3, 0,
		AssimpMatrix.c1, AssimpMatrix.c2, AssimpMatrix.c3, 0,
		0,0, 0, 1);
	m = DirectX::XMMatrixTranspose(m);
	return m;
}

void AddBoneData(Vertex &vert, UINT boneID, float weight) {
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



Mesh Model::processMesh(aiMesh * mesh, aiScene * scene)
{
	Mesh *curMeshP;
	curMeshP = new Mesh;
	Mesh &curMesh = *curMeshP;
	curMesh.globalInverseTransform = this->globalInverseTransform;
	curMesh.device = device;
	curMesh.scene = scene;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
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

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++)
			curMesh.indices.push_back(face.mIndices[j]);
	}


	if (mesh->HasBones()) {
		this->numBones = 0;
		for (unsigned int i = 0; i < mesh->mNumBones; i++) {
			unsigned int boneIndex = 0;
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

			for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
				unsigned int vertID = mesh->mBones[i]->mWeights[j].mVertexId;
				float weight = mesh->mBones[i]->mWeights[j].mWeight;
				AddBoneData(curMesh.vertices[vertID], boneIndex, weight);
			}
		}
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
		curMesh.textures.insert(curMesh.textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		//std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal", scene);
		//curMesh.textures.insert(curMesh.textures.end(), normalMaps.begin(), normalMaps.end());
		
	}
	curMesh.MeshInit();
	return curMesh;
}

void Model::processNode(aiNode * node, aiScene * scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(this->processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		this->processNode(node->mChildren[i], scene);
	}
}


std::vector<Texture> Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName,  aiScene * scene)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString texture_file;
		mat->GetTexture(type, i, &texture_file);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < loadedTextures.size(); j++) {
			if (std::strcmp(loadedTextures[j].path.c_str(), texture_file.C_Str()) == 0) {
				textures.push_back(loadedTextures[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip) {   // If texture hasn't been loaded already, load it
			HRESULT hr;
			Texture texture;
			if (auto texType = scene->GetEmbeddedTexture(texture_file.C_Str())) {
				int textureIndex = getTextureIndex(&texture_file);
				hr = DirectX::CreateWICTextureFromMemory(device, devCon, (const uint8_t*)(scene->mTextures[textureIndex]->pcData), scene->mTextures[textureIndex]->mWidth, nullptr, texture.texture.GetAddressOf());
				if (FAILED(hr))
					MessageBox(hwnd, L"Texture couldn't be created from memory!", L"Error!", MB_ICONERROR | MB_OK);
				
			}
			else {
				std::string filename = std::string(texture_file.C_Str());
				filename = filePath + '/' + filename;
				std::wstring filenamews = std::wstring(filename.begin(), filename.end());
				hr = DirectX::CreateWICTextureFromFile(device, devCon, filenamews.c_str(), nullptr, texture.texture.GetAddressOf());
				if (FAILED(hr))
					MessageBox(hwnd, L"Texture couldn't be loaded", L"Error!", MB_ICONERROR | MB_OK);
			}
			texture.type = typeName;
			texture.path = texture_file.C_Str();
			textures.push_back(texture);
			loadedTextures.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

int Model::getTextureIndex(aiString * str)
{
	std::string tistr;
	tistr = str->C_Str();
	tistr = tistr.substr(1);
	return stoi(tistr);
}