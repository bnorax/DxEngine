#pragma once
#include <directXmath.h>
	struct SimpleVertex {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texCoord;
		DirectX::XMFLOAT3 normal;
	};

	class MyMesh {
	private:
	public:
		bool staticMesh = false;
		std::vector<SimpleVertex> verts;
		std::vector<WORD> ind;
		std::shared_ptr<ID3D11ShaderResourceView*> texture = nullptr;
		void LoadTexture(const wchar_t* filename, ID3D11Device* device);
		MyMesh() {
			//auto texture = std::make_shared<ID3D11ShaderResourceView*>;
		}
	};