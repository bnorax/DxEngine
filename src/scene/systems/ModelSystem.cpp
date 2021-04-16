#include <dxpch.h>

#include "scene/components/Model.h"
#include "renderer/Renderer.h"
//#include "scene/components/Renderable.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "WICTextureLoader.h"

void DxEngine::SceneNS::ModelSystem::MeshInit(Mesh& mesh) {
	HRESULT hr;
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mesh.vertices.size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &mesh.vertices[0];

	hr = renderer.device->CreateBuffer(&vbd, &initData, mesh.vertexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to create vertex buffer.");
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = static_cast<UINT>(sizeof(UINT) * mesh.indices.size());
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	initData.pSysMem = &mesh.indices[0];
	hr = renderer.device->CreateBuffer(&ibd, &initData, mesh.indexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to create index buffer.");
	}
}

//void DxEngine::SceneNS::ModelSystem::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const DirectX::XMMATRIX& ParentTransform)
//{
//	std::string NodeName(pNode->mName.data);
//
//	const aiAnimation* pAnimation = scene->mAnimations[0];
//	DirectX::XMMATRIX NodeTransformation = aiToXMMATRIX(pNode->mTransformation);
//	//aiNodeAnim* pNodeAnim = animMap->at(pNode->mName);
//	//const aiNodeAnim* pNodeAnim = paretnModel.animMap.find(NodeName);
//	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
//	if (pNodeAnim) {
//		// Interpolate scaling and generate scaling transformation matrix
//		aiVector3D Scaling;
//		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
//		//XMFLOAT4X4 scaleTransform = InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);
//		DirectX::XMMATRIX scaleM = DirectX::XMMatrixScaling(Scaling.x, Scaling.y, Scaling.z);
//
//		// Interpolate rotation and generate rotation transformation matrix
//		aiQuaternion RotationQ;
//		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
//		//DirectX::XMVECTOR r = DirectX::XMVectorSet(RotationQ.w, RotationQ.x, RotationQ.y, RotationQ.z);
//		DirectX::XMMATRIX rotM = aiToXMMATRIX(RotationQ.GetMatrix());
//		//DirectX::XMMATRIX rotM = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(RotationQ.x, RotationQ.y, RotationQ.z, RotationQ.w));
//
//		// Interpolate translation and generate translation transformation matrix
//		aiVector3D Translation;
//		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
//		DirectX::XMMATRIX translM = DirectX::XMMatrixTranslation(Translation.x, Translation.y, Translation.z);
//		// Combine the above transformations
//		NodeTransformation = scaleM * rotM * translM;
//		//NodeTransformation = DirectX::XMMatrixTranspose(NodeTransformation);
//	}
//
//	DirectX::XMMATRIX GlobalTransformation = NodeTransformation * ParentTransform;
//	//XMMATRIX GlobalTransformation =ParentTransform* NodeTransformation ;
//
//	if (boneMap.find(NodeName) != boneMap.end()) {
//		UINT BoneIndex = boneMap[NodeName];
//		boneList[BoneIndex].finalTransform = boneList[BoneIndex].offsetMat * GlobalTransformation * globalInverseTransform;
//		//boneList[BoneIndex].finalTransform = globalInverseTransform *  GlobalTransformation * boneList[BoneIndex].offsetMat;
//	}
//
//	for (UINT i = 0; i < pNode->mNumChildren; i++) {
//		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
//	}
//}
//
//void BoneTransform(float timeInSeconds)
//{
//	DirectX::XMMATRIX fullBoneTransform = DirectX::XMMatrixIdentity();
//	float TicksPerSecond = static_cast<float>(scene->mAnimations[0]->mTicksPerSecond != 0 ?
//		scene->mAnimations[0]->mTicksPerSecond : 25.0f);
//	float TimeInTicks = timeInSeconds * TicksPerSecond;
//	float AnimationTime = fmod(TimeInTicks, (float)scene->mAnimations[0]->mDuration);
//	//float AnimationTime = scene->mAnimations[0]->mDuration / scene->mAnimations[0]->mTicksPerSecond;
//	ReadNodeHeirarchy(AnimationTime, scene->mRootNode, fullBoneTransform);
//
//}

UINT FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (UINT i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


UINT FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (UINT i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


UINT FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (UINT i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	UINT PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	UINT NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	UINT RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	UINT NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}


void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	UINT ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	UINT NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


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
		0, 0, 0, 1);
	m = DirectX::XMMatrixTranspose(m);
	return m;
}

void AddBoneData(DxEngine::Vertex& vert, UINT boneID, float weight) {
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

int DxEngine::SceneNS::ModelSystem::getTextureIndex(aiString* str)
{
	std::string tistr;
	tistr = str->C_Str();
	tistr = tistr.substr(1);
	return stoi(tistr);
}

std::vector<std::shared_ptr<DxEngine::Texture>> DxEngine::SceneNS::ModelSystem::loadMaterialTextures(Components::Model& data, aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene)
{
	std::vector<std::shared_ptr<Texture>> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString texture_file;
		mat->GetTexture(type, i, &texture_file);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < data.loadedTextures.size(); j++) {
			if (std::strcmp(data.loadedTextures[j]->path.c_str(), texture_file.C_Str()) == 0) {
				textures.push_back(data.loadedTextures[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip) {   // If texture hasn't been loaded already, load it
			HRESULT hr;
			Texture texture;
			if (auto texType = scene->GetEmbeddedTexture(texture_file.C_Str())) {
				int textureIndex = getTextureIndex(&texture_file);
				hr = DirectX::CreateWICTextureFromMemory(renderer.device.Get(), renderer.deviceContext.Get(), (const uint8_t*)(scene->mTextures[textureIndex]->pcData), scene->mTextures[textureIndex]->mWidth, nullptr, texture.texture.GetAddressOf());
			}
			/*else {
				std::string filename = std::string(texture_file.C_Str());
				filename = filePath + '/' + filename;
				std::wstring filenamews = std::wstring(filename.begin(), filename.end());
				hr = DirectX::CreateWICTextureFromFile(device, devCon, filenamews.c_str(), nullptr, texture.texture.GetAddressOf());
			}*/
			texture.type = typeName;
			texture.path = texture_file.C_Str();
			textures.push_back(std::make_shared<Texture>(texture));
			data.loadedTextures.push_back(std::make_shared<Texture>(texture));  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}


std::shared_ptr<DxEngine::Mesh> DxEngine::SceneNS::ModelSystem::processMesh(Components::Model &data, aiMesh* mesh, const aiScene* scene)
{
	std::shared_ptr<Mesh> curMesh = std::make_shared<Mesh>();
	//curMesh.InverseTransform = this->globalInverseTransform;
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

		curMesh->vertices.push_back(vertex);
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++)
			curMesh->indices.push_back(face.mIndices[j]);
	}

	if (mesh->HasBones()) {
		int numBones = 0;
		for (unsigned int i = 0; i < mesh->mNumBones; i++) {
			unsigned int boneIndex = 0;
			std::string boneName(mesh->mBones[i]->mName.data);
			if (curMesh->boneMap.find(boneName) == curMesh->boneMap.end()) {
				boneIndex = numBones;
				numBones++;
				Bone bone;
				curMesh->boneList.push_back(bone);
				//bone.offsetMat = aiToXMMATRIX(mesh->mBones[i]->mOffsetMatrix);
				//curMesh.boneMap.insert(std::make_pair(boneName, boneIndex));
			}
			else {
				boneIndex = curMesh->boneMap[boneName];
			}
			curMesh->boneMap[boneName] = boneIndex;
			curMesh->boneList[boneIndex].offsetMat = aiToXMMATRIX(mesh->mBones[i]->mOffsetMatrix);

			for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
				unsigned int vertID = mesh->mBones[i]->mWeights[j].mVertexId;
				float weight = mesh->mBones[i]->mWeights[j].mWeight;
				AddBoneData(curMesh->vertices[vertID], boneIndex, weight);
			}
		}
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<std::shared_ptr<Texture>> diffuseMaps = loadMaterialTextures(data, material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
		curMesh->textures.insert(curMesh->textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		//std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal", scene);
		//curMesh.textures.insert(curMesh.textures.end(), normalMaps.begin(), normalMaps.end());

	}
	MeshInit(*curMesh);
	return curMesh;
}

void DxEngine::SceneNS::ModelSystem::processNode(aiNode* node, const aiScene* scene, Components::Model& data) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		data.meshes.push_back(this->processMesh(data, mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		this->processNode(node->mChildren[i], scene, data);
	}
}

bool DxEngine::SceneNS::ModelSystem::Load(entt::registry& registry)
{
	Assimp::Importer importer;

	auto view = registry.view<Components::Model>();
	for (auto entity : view) {
		auto component = view.get<Components::Model>(entity);
		importer.ReadFile(component.filePath,
			aiProcess_Triangulate |
			aiProcess_RemoveRedundantMaterials | // remove redundant materials
			aiProcess_FindDegenerates | // remove degenerated polygons from the import
			aiProcess_FindInvalidData |
			aiProcess_SortByPType |
			aiProcess_FlipUVs |
			aiProcess_ConvertToLeftHanded);
		const aiScene* loadedScene = importer.GetScene();
		if (loadedScene == nullptr) {
			return false;
		}
		component.modelInverseTransform = aiToXMMATRIX(loadedScene->mRootNode->mTransformation);
		processNode(loadedScene->mRootNode, loadedScene, component);
		registry.replace<Components::Model>(entity, component);
	}
	return true;
}

void DxEngine::SceneNS::ModelSystem::Draw(entt::registry& registry) {
	auto view = registry.view<Components::Model>();
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	for (auto entity : view) {
		auto model = view.get<Components::Model>(entity);///////
		if (registry.has<Components::Skybox>(entity)) {
			auto mesh = model.meshes[0];
			renderer.deviceContext->RSSetState(renderer.renderStates->rasterizerState.Get());
			renderer.deviceContext->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
			renderer.deviceContext->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			ConstantBufferPerObject cb;
			if (!mesh->textures.empty())
				renderer.deviceContext->PSSetShaderResources(1, 1, mesh->textures[0]->texture.GetAddressOf());
			cb.world = XMMatrixTranspose(renderer.wvp.world);
			renderer.deviceContext->UpdateSubresource(renderer.renderStates->cbPerObject.Get(), 0, nullptr, &cb, 0, 0);
			renderer.deviceContext->DrawIndexed(static_cast<unsigned int>(mesh->indices.size()), 0, 0);
			continue;
		}
		for (auto mesh : model.meshes) {
			renderer.deviceContext->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
			renderer.deviceContext->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
			ConstantBufferPerObject cb;
			if (!mesh->textures.empty())
				renderer.deviceContext->PSSetShaderResources(1, 1, mesh->textures[0]->texture.GetAddressOf());
			//BonesCB bonesCB;
			//float a = (float)time.GetTimeSinceAppStart();
			//BoneTransform(a / 1000);
			//for (UINT i = 0; i < boneList.size() && i < 500; i++) {
			//	bonesCB.bones[i] = XMMatrixTranspose(boneList[i].finalTransform);
				//bonesCB.bones[i] = boneList[i].finalTransform;
			//}
			cb.world = XMMatrixTranspose(renderer.wvp.world);
			renderer.deviceContext->UpdateSubresource(renderer.renderStates->cbPerObject.Get(), 0, nullptr, &cb, 0, 0);
			//renderer.deviceContext->UpdateSubresource(g_pConstantBufferBones, 0, nullptr, &bonesCB, 0, 0);
			renderer.deviceContext->DrawIndexed(static_cast<unsigned int>(mesh->indices.size()), 0, 0);
		}
		//renderer.deviceContext->DrawIndexed()
	}

}