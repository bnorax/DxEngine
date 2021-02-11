#include <dxpch.h>
#include "ReadObj.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "InitBuff.h"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


HRESULT RedObjFromFileAssimp(const char* fileName, MyMesh **out) {
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(fileName,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);
	if (!scene) {
		return E_FAIL;
	}
	aiMesh *mesh = scene->mMeshes[0];
	MyMesh *mMesh = new MyMesh;
	for (int i = 0; i< mesh->mNumVertices; i++) {
		SimpleVertex sv;
		sv.pos.x = mesh->mVertices[i].x;
		sv.pos.y = mesh->mVertices[i].y;
		sv.pos.z = mesh->mVertices[i].z;
		sv.color = DirectX::XMFLOAT4(0.4f, 0.1f, 0.8f, 1.0f);
		mMesh->verts.push_back(sv);
	}
	for (int i = 0; i<mesh->mNumFaces; i++) {
		for (int j = 0; j< mesh->mFaces->mNumIndices; j++) {
			mMesh->ind.push_back(mesh->mFaces[i].mIndices[j]);
		}
	}
	*out = mMesh;
	return S_OK;
}



HRESULT ReadObjFromFile(const char* fileName, MyMesh **out) {
	std::ifstream objFile(fileName);
	std::string temp, temp2;
	std::vector<std::string> splits, subSplits;
	std::vector<SimpleVertex> vertList;
	std::vector<DirectX::XMFLOAT2> texDataList;
	std::vector<DirectX::XMFLOAT3> normalDataList;
	std::vector<WORD> vertIndsList;
	std::vector<WORD> texIndsList;
	SimpleVertex vert;
	MyMesh *object = new MyMesh;
	int verticesI = 0;
	int indicesI = 0;


	if (objFile.is_open()) {
		while (getline(objFile, temp)) {
			boost::split(splits, temp, boost::is_any_of(" "));
			if (splits[0] == "v") {
				vert.pos.x = boost::lexical_cast<float>(splits[1]);
				vert.pos.y = boost::lexical_cast<float>(splits[2]);
				vert.pos.z = boost::lexical_cast<float>(splits[3]);
				vert.color = DirectX::XMFLOAT4 (0.4f, 0.1f, 0.8f, 1.0f);
				vertList.push_back(vert);
				continue;
			}
			if (splits[0] == "vt") {
				DirectX::XMFLOAT2 newD;
				newD.x = boost::lexical_cast<float>(splits[1]);
				newD.y = boost::lexical_cast<float>(splits[2]);
				texDataList.push_back(newD);
				continue;
			}
			if (splits[0] == "vn") {
				DirectX::XMFLOAT3 newNormal;
				newNormal.x = boost::lexical_cast<float>(splits[1]);
				newNormal.y = boost::lexical_cast<float>(splits[2]);
				newNormal.z = boost::lexical_cast<float>(splits[3]);
				normalDataList.push_back(newNormal);
				continue;
			}
			if (splits[0] == "f") {
				for (int i = 1; i < splits.size(); i++) {
					temp2 = splits[i];
					boost::split(subSplits, temp2, boost::is_any_of("/"));
					if (subSplits.size() == 1) {
						WORD ind1 = boost::lexical_cast<WORD>(subSplits[0]) - 1;
						vert.pos.x = vertList[ind1].pos.x;
						vert.pos.y = vertList[ind1].pos.y;
						vert.pos.z = vertList[ind1].pos.z;
						vert.color = vertList[ind1].color;
					}
					if (subSplits.size() == 2) {
						WORD ind1 = boost::lexical_cast<WORD>(subSplits[0]) - 1;
						WORD ind2 = boost::lexical_cast<WORD>(subSplits[1]) - 1;
						vert.pos.x = vertList[ind1].pos.x;
						vert.pos.y = vertList[ind1].pos.y;
						vert.pos.z = vertList[ind1].pos.z;
						vert.color = vertList[ind1].color;
						vert.texCoord.y = -texDataList[ind2].y;
						vert.texCoord.x = texDataList[ind2].x;
					}
					if (subSplits.size() == 3) {
						WORD ind1, ind2, ind3;
						ind1 = boost::lexical_cast<WORD>(subSplits[0]) - 1;
						if (subSplits[1] != "") {
							ind2 = boost::lexical_cast<WORD>(subSplits[1]) - 1;
							vert.texCoord.y = -texDataList[ind2].y;
							vert.texCoord.x = texDataList[ind2].x;
						}
						if (subSplits[2] != "") {
							ind3 = boost::lexical_cast<WORD>(subSplits[2]) - 1;
							vert.normal = normalDataList[ind3];
						}
						vert.pos.x = vertList[ind1].pos.x;
						vert.pos.y = vertList[ind1].pos.y;
						vert.pos.z = vertList[ind1].pos.z;
						vert.color = vertList[ind1].color;
					}

					int index = -1;
					for (int i = 0; i < verticesI; i++) {
						if (memcmp(&object->verts[i], &vert, sizeof(SimpleVertex)) == 0) index = i;
					}
					//Добавление
					if (index < 0)
					{
						
						object->verts.push_back(vert);
						verticesI++;
						index = verticesI-1;
					}
					object->ind.push_back(index);
					indicesI++;
				}
			}
			
		}
		*out = object;
		return S_OK;
	}
	return E_FAIL;

}