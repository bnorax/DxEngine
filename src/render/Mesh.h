#pragma once
#include <directXmath.h>
	struct VertexBoneInfo {//ошибка
		std::vector<UINT> boneIDs;
		std::vector<float> weights;
	};

	struct SimpleVertex {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texCoord;
		DirectX::XMFLOAT3 normal;
		std::vector<VertexBoneInfo> bones;
	};

	struct Texture {
		std::string type;
		std::string path;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture = nullptr;
	};

	struct Animation {
		std::string a;
	};

	class Mesh {
	public:
		bool staticMesh = false;
		std::vector<SimpleVertex> vertices;
		std::vector<UINT> indices;
		std::vector<Texture> textures;
		ID3D11Device *device;
		Mesh(ID3D11Device *dev, const std::vector<SimpleVertex> vert, const std::vector<UINT> ind, const std::vector<Texture>& tex);
		void Draw(ID3D11DeviceContext *devcon);
		void LoadBones();
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, indexBuffer;

		
	};