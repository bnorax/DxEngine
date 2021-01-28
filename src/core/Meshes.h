//--------------------------------------------------------------------------------------
// ������������ ����, ���������� �������� ��������, ������� � �������,
// ��������� ��� ������ � DirectX
// ��� ��� ������������� ����� ���������� � ������� ����� ���� Meshes.cpp
//
// �� ������ ������� � ������� ������ (������� ������������ � �����������)
// ����� ��������� ���������� ����������, �������� � namespace Meshes,
// �������� �������� ��������� DirectX 
// ��������:
//		Meshes::g_pd3dDevice = g_pd3dDevice;
//		Meshes::g_pImmediateContext = g_pImmediateContext;
//		Meshes::g_pConstantBuffer = g_pConstantBuffer;
//		Meshes::P_cb = &cb;				
//		Meshes::Size_cb = sizeof(cb);		
//		Meshes::g_pWorld = &cb.world;
//		Meshes::ConstantBufferRegisterIndex = 0;
//--------------------------------------------------------------------------------------

#ifndef MESHES
#define MESHES

#include <windows.h>
#include <d3d11.h>
//#include <d3dx11.h>
#include <d3dcompiler.h>
//#include <xnamath.h>
#include <DirectXMath.h>
#include<assert.h>

//--------------------------------------------------------------------------------------
// ���������� ���������� ������ (namespace Meshes)
// �� ������ ���� ��������� ������ ��������� �������� �� ������ ����������� ������� � 
// ������� ������ (������� ������������ � �����������)
//
// ��� ������������� ���������� ��������� ��� �������, ����� ������ �������� ���� 
// ���������� ����������, ������� ��������������� �������� ����� ������� �������.
// ������ ���������� ������, ��������� ��� �������� ��� ���������� � ���������� �����������, 
// �� ����� ���� ���������� �� ������ ����������
//--------------------------------------------------------------------------------------
namespace Meshes
{
	extern ID3D11Device* g_pd3dDevice;					// ���������� (��� �������� ��������)
	extern ID3D11DeviceContext* g_pImmediateContext;	// �������� (���������� ���������)

	// ��������� ���������� ������������ ������ � ��������, �������� ������� ����
	// (��������� ������� Draw, �� ����������� Draw_0; � ����� ������� SetBuffers() )
	extern ID3D11Buffer* g_pConstantBuffer;				// ����������� �����, ����������� ������ � �������� ����
	extern void* P_cb;				// ��������� �� ���������, ������������ ��� ���������� ����� ������
	extern int Size_cb;				// ������ ���� ��������� � ������ 
	extern DirectX::XMMATRIX* g_pWorld;		// ��������� �� ������� ����, �������� � ������ ���� ���������
	extern int ConstantBufferRegisterIndex;	// ������ �������� b (� GPU), ���� ���������� ���� ����������� �����
}

//--------------------------------------------------------------------------------------
// ��������� �������. ����������� �������� � ��� ���������, �� ��� ����� 
// ����������� ����� ������ ��������� ���� ������� � ����� Meshes.cpp
//--------------------------------------------------------------------------------------
struct VERTEX
{
	DirectX::XMFLOAT3 Pos;		// ���������� ����� � ������������
	DirectX::XMFLOAT3 TexPos;	// ���������� ����� � ��������
	DirectX::XMFLOAT3 Norm;		// ������� � ����������� � ������ �����
	DirectX::XMFLOAT2 TexNormPos; //���������� ����� � "��������" �������� (��� ��������� ���������� ��������)
};

    // ����������� ������� ������					// (����� const ���������� �� ��������� ������ �������, �.�. 
const D3D11_INPUT_ELEMENT_DESC layout[] =			// ���� ������������ ���� ������������ ������������ � ������ �������) 
{													
	{ "POSITION",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXPOSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",		 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXNORMAL",	 0, DXGI_FORMAT_R32G32_FLOAT,	 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
const UINT numElements = ARRAYSIZE( layout );



//--------------------------------------------------------------------------------------
// ������� ��� ���������� ��������, �������� � ��������� �������
//--------------------------------------------------------------------------------------

	// ��������������� ������� ��� ���������� �������� � D3DX11 (����� �� ��������� ��� ���������,
	//  �� ������ �������� ����������� ���������������� �����; �� ���������� Meshes.h)
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut );

	// ������� ��� ���������� � �������� ���������� ��������, � ����� �������� ������� ������
HRESULT CreateVertexShader(WCHAR * szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, 
	ID3D11VertexShader ** ppVertexShader, ID3D11InputLayout** ppVertexLayout);

	// ������� ��� ���������� � �������� ���������� ��������
HRESULT CreatePixelShader(WCHAR * szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, 
	ID3D11PixelShader ** ppPixelShader);

	// ��������������� ������� ��� �������� ����� �������
HRESULT CreateBuffer(void* source, int size, UINT BindFlags, ID3D11Buffer** destination);

	// ������� ��������� ������� (����������, ���������� � ������������) � ����������� ������� 
	// ��� ������ ��������� ������� (���������� � ������ Draw(), �� ����� �������������� � ��������)
void SetBuffers(ID3D11Buffer* VertexBuffer, ID3D11Buffer* IndexBuffer, ID3D11PixelShader* PixelShader);



//--------------------------------------------------------------------------------------
// �������� ����� ��� �����
//--------------------------------------------------------------------------------------
class Mesh
{
public:
		// ������������ � �����������
	Mesh();
	Mesh(const VERTEX *Vertices, int NVertices, const WORD *Indices, int NIndices, int SetNormal = 0, 
		ID3D11PixelShader *PixelShader = NULL);
	Mesh(const char * FileName, ID3D11PixelShader *PixelShader = NULL);
	~Mesh();

		// ������������ ������
	void Release();

		// ������� ����������
	void SetVerticesIndices(const VERTEX *Vertices, int NVertices, const WORD *Indices, int NIndices, int SetNormal = 0);
	void CalculateNormals();
	void CreateVerticesIndicesBuffers();
	void SetWorld(DirectX::CXMMATRIX World);
	void SetPixelShader(ID3D11PixelShader *PixelShader);

		// ���������� ����������
	VERTEX * GetVertices();
	WORD * GetIndices();
	int GetNVertices();
	int GetNIndices();
	ID3D11Buffer * GetVertexBuffer();
	ID3D11Buffer * GetIndexBuffer();
	DirectX::XMMATRIX GetWorld();
	ID3D11PixelShader * GetPixelShader();

		// �������� ���� �� �����
	void LoadFromObj(const char * FileName);		// �������� ���� �� ����� .obj

		// ���������� �� ������
	void Draw_0();					// ������� ���� �� ���������� (������������ ������� ��������);
									// �� ��������������� ������ � ���������� ������ (� ������� �� ������ ������� ����������)
	void Draw();					// ������� ���� = ���� world
	void Draw(DirectX::CXMMATRIX World);		// ������� ���� = (���� world) * (�������� World) , ����� ������ �� ���

protected:
	VERTEX *vertices;
	WORD *indices;
	int nvertices, nindices;
	ID3D11Buffer *vertexbuffer, *indexbuffer;
	ID3D11PixelShader *pixelshader;
	DirectX::XMFLOAT4X4 world;

	void Clear();
};


//--------------------------------------------------------------------------------------
// ����� ��� ������
//--------------------------------------------------------------------------------------
class Cone : public Mesh
{
public:
	Cone(double h, double r, int ny, int nFi);
};


//--------------------------------------------------------------------------------------
// ����� ��� �������������� � �������
//--------------------------------------------------------------------------------------
class TextMesh : public Mesh
{
public:
	void Print(char * Text);

		// ����� �� ������: 
		// hRel1, hRel2 - ������������� ������ �������� � ������� ���� ������ (� ����� ������ ������), 
		// fi0 - ������� ��������� ������ ���� ������ (������������� �� ��� X ������ ������� �������)
		// Magnify - ����������� ���������� ������� ������ �� ��������� � ������� (����� �� �� ���������� �������)
	void PrintCone(char * Text, Cone & C, double hRel1, double hRel2, double fi0, double Magnify);

		// ����� �� ������������ ����������� ��������:
		// h1, h2 - ���������� �������� � ������� ���� ������ �� ��� Y,
		// r1, r2 - ������� ���������� ��� �������� � ������� ����� ������ (������ ��� Y)
		// fi0 - ������� ��������� ������ ���� ������ (������������� �� ��� X ������ ������� �������)
	void PrintCone(char * Text, double h1, double r1, double h2, double r2, double fi0);	
};


//--------------------------------------------------------------------------------------
// ����� ��� �������� (���������)
//--------------------------------------------------------------------------------------
class Instance
{
public:
		// ��������� �� ���, ��������������� ������ ���������
	Mesh * mesh;

		// ������������ � �����������
	Instance(); 
	Instance(Mesh * M); 
	Instance(Mesh * M, DirectX::CXMMATRIX World);
	~Instance();

		// ������� ����������
	void SetWorld(DirectX::CXMMATRIX World);

		// ���������� ����������
	DirectX::XMMATRIX GetWorld();

		// ���������� �� ������
	void Draw_0();		// ������� ���� �� ���������� (������������ ������� ��������); 
						// �� ��������������� ������ � ���������� ������
	void Draw();		// ������� ���� = (���� world ����) * (���� world ���������) , ����� ������ �� ���; 
						// ��������������� ������ � ���������� ������
			// ��� ����� ������� �������� � ��������� ����� ����������� ����� ����� ���� mesh

protected:
	DirectX::XMFLOAT4X4 world;
};

#endif
