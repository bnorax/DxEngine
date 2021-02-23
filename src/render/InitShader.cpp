#include <dxpch.h>
#include <d3dcompiler.h>
#include "InitDevice.h"

ID3D11VertexShader* vertex_shader_ptr = NULL;
ID3D11VertexShader* vertex_wave_ptr = NULL;
ID3D11PixelShader* pixel_shader_ptr = NULL;
ID3D11PixelShader* pixel_nt_shader_ptr = NULL;
ID3D11InputLayout* input_layout_ptr = NULL;
ID3D11PixelShader* pixel_wave_ptr = NULL;

void InitShader() {
	ID3DBlob *vs_blob_ptr = NULL, *ps_blob_ptr = NULL, *ps_nt_blob_ptr = NULL, *ps_wave_blob_ptr = NULL, *vs_wave = NULL , *errorMessages = nullptr;;
	const wchar_t* vs =  L"resources/shader/VertexShader.hlsl";
	const wchar_t* ps = L"resources/shader/PixelShader.hlsl";
	// load and compile the two shaders
	D3DCompileFromFile(
		vs,
		nullptr,
		NULL,
		"vs_main",
		"vs_5_0",
		NULL,
		0,
		&vs_blob_ptr,
		NULL);
	D3DCompileFromFile(
		ps,
		nullptr,
		NULL,
		"ps_main",
		"ps_5_0",
		NULL,
		0,
		&ps_blob_ptr,
		NULL);
	D3DCompileFromFile(
		ps,
		nullptr,
		NULL,
		"ps_main_notexture",
		"ps_5_0",
		NULL,
		0,
		&ps_nt_blob_ptr,
		NULL);
	HRESULT r = D3DCompileFromFile(
		ps,
		nullptr,
		NULL,
		"ps_wave_dist",
		"ps_5_0",
		NULL,
		0,
		&ps_wave_blob_ptr,
		&errorMessages);
	r = D3DCompileFromFile(
		vs,
		nullptr,
		NULL,
		"vs_wave",
		"vs_5_0",
		NULL,
		0,
		&vs_wave,
		&errorMessages);
	//const char* errorMsg = (const char*)errorMessages->GetBufferPointer();
	HRESULT hr;
	g_pd3dDevice->CreateVertexShader(vs_blob_ptr->GetBufferPointer(), vs_blob_ptr->GetBufferSize(), NULL, &vertex_shader_ptr);
	g_pd3dDevice->CreateVertexShader(vs_wave->GetBufferPointer(), vs_wave->GetBufferSize(), NULL, &vertex_wave_ptr);
	g_pd3dDevice->CreatePixelShader(ps_blob_ptr->GetBufferPointer(), ps_blob_ptr->GetBufferSize(), NULL, &pixel_shader_ptr);
	g_pd3dDevice->CreatePixelShader(ps_nt_blob_ptr->GetBufferPointer(), ps_nt_blob_ptr->GetBufferSize(), NULL, &pixel_nt_shader_ptr);
	g_pd3dDevice->CreatePixelShader(ps_wave_blob_ptr->GetBufferPointer(), ps_wave_blob_ptr->GetBufferSize(), NULL, &pixel_wave_ptr);
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
	  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{"BONEID", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{"BONEWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	hr = g_pd3dDevice->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), vs_blob_ptr->GetBufferPointer(), vs_blob_ptr->GetBufferSize(), &input_layout_ptr);
	g_pd3dDeviceContext->IASetInputLayout(input_layout_ptr);
}