#include <dxpch.h>
#include "Mesh.h"

#include <WICTextureLoader.h>


Mesh::Mesh()
{
}

Mesh::Mesh(aiScene *ascene, ID3D11Device *dev, const std::vector<SimpleVertex> vert, const std::vector<UINT> ind, const std::vector<Texture>& tex, const std::map<std::string, UINT> boneM, const std::vector<Bone> boneL) :
		vertices(vert),
		indices(ind),
		textures(tex),
		device(dev),
		boneList(boneL),
		boneMap(boneM),
		scene(ascene)
{
	HRESULT hr;
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = static_cast<UINT>(sizeof(SimpleVertex) * vertices.size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &vertices[0];

	hr = dev->CreateBuffer(&vbd, &initData, vertexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to create vertex buffer.");
	}

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = static_cast<UINT>(sizeof(UINT) * indices.size());
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	initData.pSysMem = &indices[0];
	hr = dev->CreateBuffer(&ibd, &initData, indexBuffer.GetAddressOf());
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to create index buffer.");
	}
}

void Mesh::BoneTransform(float timeInSeconds)
{
	DirectX::XMMATRIX fullBoneTransform = XMMatrixIdentity();
	//float TicksPerSecond = scene->mAnimations[0]->mTicksPerSecond != 0 ?
	//	scene->mAnimations[0]->mTicksPerSecond : 25.0f;
	//float TimeInTicks = timeInSeconds * TicksPerSecond;
	float AnimationTime = scene->mAnimations[0]->mDuration / scene->mAnimations[0]->mTicksPerSecond;

	ReadNodeHeirarchy(AnimationTime, scene->mRootNode, fullBoneTransform);

}

UINT Mesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (UINT i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


UINT Mesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
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


UINT Mesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
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


void Mesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
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


void Mesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
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


void Mesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
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


void Mesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const XMMATRIX& ParentTransform)
{
	std::string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = scene->mAnimations[0];
	XMMATRIX NodeTransformation = aiToXMMATRIX(pNode->mTransformation);
	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		//XMFLOAT4X4 scaleTransform = InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);
		DirectX::XMMATRIX scaleM = DirectX::XMMatrixScaling(Scaling.x, Scaling.y, Scaling.z);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		DirectX::XMMATRIX rotM = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(RotationQ.x, RotationQ.y, RotationQ.z, RotationQ.w));

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		DirectX::XMMATRIX translM = DirectX::XMMatrixTranslation(Translation.x, Translation.y, Translation.z);
		// Combine the above transformations
		NodeTransformation = translM * rotM * scaleM;
	}

	XMMATRIX GlobalTransformation = ParentTransform * NodeTransformation;

	if (boneMap.find(NodeName) != boneMap.end()) {
		UINT BoneIndex = boneMap[NodeName];
		boneList[BoneIndex].finalTransform = meshInitTransform * GlobalTransformation * boneList[BoneIndex].offsetMat;
	}

	for (UINT i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}


const aiNodeAnim* Mesh::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
	for (UINT i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}


void Mesh::Draw(ID3D11DeviceContext *devcon) {
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	devcon->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	devcon->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	for (int i = 0; i < textures.size(); i++) {
		devcon->PSSetShaderResources(i +1, 1, textures[i].texture.GetAddressOf());
	}
	ConstantBuffer cb;
	BonesCB bonesCB;
	BoneTransform(10);
	for (UINT i = 0; i < boneList.size() && i<1000; i++) {
		bonesCB.bones[i] = XMMatrixTranspose(boneList[i].finalTransform);
	}
	//for (UINT i = boneList.size(); i < 100 - boneList.size(); i++) {
	//	//bonesCB.bones[i] = DirectX::XMMatrixIdentity();
	//}
	cb.mWorld = XMMatrixTranspose(g_World);
	cb.mView = XMMatrixTranspose(g_View);
	cb.mProjection = XMMatrixTranspose(g_Projection);
	g_pd3dDeviceContext->UpdateSubresource(g_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	g_pd3dDeviceContext->UpdateSubresource(g_pConstantBufferBones, 0, nullptr, &cb, 0, 0);
	devcon->DrawIndexed(static_cast<UINT>(indices.size()), 0, 0);
}

//void Mesh::LoadTexture(const wchar_t* filename, ID3D11Device* device) {
//	ID3D11ShaderResourceView **rawTexture = new ID3D11ShaderResourceView*;
//	CreateWICTextureFromFile(device, filename, nullptr, rawTexture);
//	texture = std::make_shared<ID3D11ShaderResourceView*>(*rawTexture);
//}