#pragma once
#include <dxpch.h>
#include <wrl/client.h>

class PixelShader{

};

class VertexShader {
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vsPtr = nullptr;

};

class Render {
public:
	std::vector<VertexShader> vertexShaders;
	std::vector<PixelShader> pixelShaders;
private:
};
