#include <dxpch.h>
#include "ModelLoader.h"
#include "InitBuff.h"

using namespace Microsoft::WRL;
//
//HRESULT ReadObjFromFile(const char* fileName, Mesh **out) {
//	std::ifstream objFile(fileName);
//	std::string temp, temp2;
//	std::vector<std::string> splits, subSplits;
//	std::vector<SimpleVertex> vertList;
//	std::vector<DirectX::XMFLOAT2> texDataList;
//	std::vector<DirectX::XMFLOAT3> normalDataList;
//	std::vector<WORD> vertIndsList;
//	std::vector<WORD> texIndsList;
//	SimpleVertex vert;
//	Mesh *object = new Mesh;
//	int verticesI = 0;
//	int indicesI = 0;
//
//
//	if (objFile.is_open()) {
//		while (getline(objFile, temp)) {
//			boost::split(splits, temp, boost::is_any_of(" "));
//			if (splits[0] == "v") {
//				vert.pos.x = boost::lexical_cast<float>(splits[1]);
//				vert.pos.y = boost::lexical_cast<float>(splits[2]);
//				vert.pos.z = boost::lexical_cast<float>(splits[3]);
//				vert.color = DirectX::XMFLOAT4 (0.4f, 0.1f, 0.8f, 1.0f);
//				vertList.push_back(vert);
//				continue;
//			}
//			if (splits[0] == "vt") {
//				DirectX::XMFLOAT2 newD;
//				newD.x = boost::lexical_cast<float>(splits[1]);
//				newD.y = boost::lexical_cast<float>(splits[2]);
//				texDataList.push_back(newD);
//				continue;
//			}
//			if (splits[0] == "vn") {
//				DirectX::XMFLOAT3 newNormal;
//				newNormal.x = boost::lexical_cast<float>(splits[1]);
//				newNormal.y = boost::lexical_cast<float>(splits[2]);
//				newNormal.z = boost::lexical_cast<float>(splits[3]);
//				normalDataList.push_back(newNormal);
//				continue;
//			}
//			if (splits[0] == "f") {
//				for (int i = 1; i < splits.size(); i++) {
//					temp2 = splits[i];
//					boost::split(subSplits, temp2, boost::is_any_of("/"));
//					if (subSplits.size() == 1) {
//						WORD ind1 = boost::lexical_cast<WORD>(subSplits[0]) - 1;
//						vert.pos.x = vertList[ind1].pos.x;
//						vert.pos.y = vertList[ind1].pos.y;
//						vert.pos.z = vertList[ind1].pos.z;
//						vert.color = vertList[ind1].color;
//					}
//					if (subSplits.size() == 2) {
//						WORD ind1 = boost::lexical_cast<WORD>(subSplits[0]) - 1;
//						WORD ind2 = boost::lexical_cast<WORD>(subSplits[1]) - 1;
//						vert.pos.x = vertList[ind1].pos.x;
//						vert.pos.y = vertList[ind1].pos.y;
//						vert.pos.z = vertList[ind1].pos.z;
//						vert.color = vertList[ind1].color;
//						vert.texCoord.y = -texDataList[ind2].y;
//						vert.texCoord.x = texDataList[ind2].x;
//					}
//					if (subSplits.size() == 3) {
//						WORD ind1, ind2, ind3;
//						ind1 = boost::lexical_cast<WORD>(subSplits[0]) - 1;
//						if (subSplits[1] != "") {
//							ind2 = boost::lexical_cast<WORD>(subSplits[1]) - 1;
//							vert.texCoord.y = -texDataList[ind2].y;
//							vert.texCoord.x = texDataList[ind2].x;
//						}
//						if (subSplits[2] != "") {
//							ind3 = boost::lexical_cast<WORD>(subSplits[2]) - 1;
//							vert.normal = normalDataList[ind3];
//						}
//						vert.pos.x = vertList[ind1].pos.x;
//						vert.pos.y = vertList[ind1].pos.y;
//						vert.pos.z = vertList[ind1].pos.z;
//						vert.color = vertList[ind1].color;
//					}
//
//					int index = -1;
//					for (int i = 0; i < verticesI; i++) {
//						if (memcmp(&object->verts[i], &vert, sizeof(SimpleVertex)) == 0) index = i;
//					}
//					//Добавление
//					if (index < 0)
//					{
//						
//						object->verts.push_back(vert);
//						verticesI++;
//						index = verticesI-1;
//					}
//					object->inds.push_back(index);
//					indicesI++;
//				}
//			}
//			
//		}
//		*out = object;
//		return S_OK;
//	}
//	return E_FAIL;
//
//}


ModelLoader::ModelLoader()
{
}

ModelLoader::~ModelLoader()
{
}

bool ModelLoader::Load(HWND hwnd, ID3D11Device * device, ID3D11DeviceContext * devCon, std::string filename)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename,
		aiProcess_Triangulate | aiProcess_RemoveRedundantMaterials | // remove redundant materials
		aiProcess_FindDegenerates | // remove degenerated polygons from the import
		aiProcess_FindInvalidData |
		aiProcess_SortByPType  |
		aiProcess_ConvertToLeftHanded |
	aiProcess_FlipUVs);
	if (scene == nullptr) return false;
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

Mesh ModelLoader::processMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<SimpleVertex> vertices;
	std::vector<UINT> indices;
	std::vector<Texture> textures;

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

		vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	}

	return Mesh(device, vertices, indices, textures);
}

void ModelLoader::processNode(aiNode * node, const aiScene * scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(this->processMesh(mesh, scene));
	}

	for (UINT i = 0; i < node->mNumChildren; i++) {
		this->processNode(node->mChildren[i], scene);
	}
}



std::vector<Texture> ModelLoader::loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName, const aiScene * scene)
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
