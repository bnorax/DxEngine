#include "Meshes.h"
#include<fstream>

using namespace DirectX;
//--------------------------------------------------------------------------------------
// ���������� ���������� ������ (namespace Meshes)
// �� ������ ���� ��������� ������ ��������� �������� �� ������ ����������� ������� � 
// ������� ������ (������� ������������ � �����������)
//--------------------------------------------------------------------------------------
namespace Meshes
{
	ID3D11Device* g_pd3dDevice = NULL;					// ���������� (��� �������� ��������)
	ID3D11DeviceContext* g_pImmediateContext = NULL;	// �������� (���������� ���������)

				// ��������� ���������� ������������ ������ � ��������, �������� ������� ����
	ID3D11Buffer* g_pConstantBuffer = NULL;				// ����������� �����, ����������� ������ � �������� ����
	void* P_cb = NULL;				// ��������� �� ���������, ������������ ��� ���������� ����� ������
	int Size_cb = 0;				// ������ ���� ��������� � ������ 
	DirectX::XMMATRIX* g_pWorld = NULL;		// ��������� �� ������� ����, �������� � ������ ���� ���������
	int ConstantBufferRegisterIndex = 0;	// ������ �������� b, ���� ������������ ���� ����������� �����
}

using namespace std;


//********************************************************************************************************
// ������ ������ Mesh
//********************************************************************************************************

void Mesh::Clear()
{
		vertices = NULL;
		indices = NULL;
		nvertices = nindices = 0;
		vertexbuffer = NULL;
		indexbuffer = NULL;
		pixelshader = NULL;
		XMStoreFloat4x4(&world, DirectX::XMMatrixIdentity());
}

//---------------------------------------------------------------------------
// ������������ � �����������
//---------------------------------------------------------------------------

Mesh::Mesh()
	{
		Clear();
	}

Mesh::Mesh(const VERTEX *Vertices, int NVertices, const WORD *Indices, int NIndices, int SetNormal, ID3D11PixelShader *PixelShader)
	{
		Clear();
		SetVerticesIndices(Vertices, NVertices, Indices, NIndices, SetNormal);
		pixelshader = PixelShader;
	}

Mesh::Mesh(const char * FileName, ID3D11PixelShader *PixelShader)
	{
		Clear();
		LoadFromObj(FileName);
		pixelshader = PixelShader;
	}


Mesh::~Mesh()
	{
		Release();
	}


void Mesh::Release()
{
		if (vertices) delete vertices;
		vertices = NULL;
		if (indices) delete indices;
		indices = NULL;
		nvertices = nindices = 0;
		if (vertexbuffer) vertexbuffer->Release();
		vertexbuffer = NULL;
		if (indexbuffer) indexbuffer->Release();
		indexbuffer = NULL;
}

//---------------------------------------------------------------------------
// ������� ������ � ������������� ���� �� ��������� ��������
//		�������� ��������� SetNormal:
//		0 (�� ���������) - �� ������������ �������, � ����������� ��������, ������������ � ������� Vertices
//		1 - ���������� ������� � ��������
//		2 - ���������� ������� � �������������; ��� ���� ����� ������ ����� �������� (�� 3 ������� ��� ������� ������������)
//---------------------------------------------------------------------------
void Mesh::SetVerticesIndices(const VERTEX *Vertices, int NVertices, const WORD *Indices, int NIndices, int SetNormal)
{
	Release();		// ������������ ������, ���� ��� ����� ���� ��������

	nindices = NIndices;
	indices = new WORD[NIndices];
	memcpy(indices, Indices, NIndices * sizeof(WORD) );

	if (SetNormal <= 1)			// ���� ��� ����� � �������� ���������� ������
	{
		nvertices = NVertices;
		vertices = new VERTEX[nvertices];
		memcpy(vertices, Vertices, NVertices * sizeof(VERTEX) );
	}
	else						// ������� �� 3 ������� ��� ������� ������������
	{
		nvertices = NIndices;
		vertices = new VERTEX[nvertices];
		for (int i=0; i<NIndices; i++)	// ����� � ������� ��������	(�������������)
		{
			int k=Indices[i];	// ����� ������� � �������� �������
			assert (k>=0 && k<NVertices);

			vertices[i] = Vertices[k];
			indices[i] = i;
		}
	}

	if (SetNormal >= 1)			// ���������� �������� ������
		CalculateNormals();

	CreateVerticesIndicesBuffers();	// ���������� �������
}

//---------------------------------------------------------------------------
// ������ ��������
//---------------------------------------------------------------------------
void Mesh::CalculateNormals()
{
	DirectX::XMVECTOR RVert[3];		// ������� ������ ������������
	int MVert[3];			// ������� ������ ������������
	int i;

// ���������� ���������, ����������� �������� �������, ������������ �� ������ �������, � ���������, ������������� �� �������� ��������
// �� ��������� ��� ��������� (�������������� DEBUG_NORMALS ����� �������� 0, ��. ����) 
// ��� �� ��������� �������� ���� ���� � ��������� #define DEBUG_NORMALS �������� 0 �� 1
// � ����� � ������ ��������� ���������������� (��� ��������)  #include<fstream>  �  using namespace std;

#define DEBUG_NORMALS 0

#if DEBUG_NORMALS == 1
	ofstream deb("DebugNormals.txt");
	deb<<"Old normals (x,y,z)     Calculated normals (x,y,z)   Difference"<<endl;
	XMFLOAT3 *DebOld = new XMFLOAT3[nvertices];
	for (i=0; i<nvertices; i++)			// ���������� ������ ������� 
		DebOld[i] = vertices[i].Norm;
#endif

	DirectX::XMFLOAT3 Zero = DirectX::XMFLOAT3(0,0,0);
	for (i=0; i<nvertices; i++)			// �������� ������� ��� ������������ ���������������
		vertices[i].Norm = Zero;		

	for (i=0; i<nindices; i++)	// ����� � ������� ��������	(�������������)
	{
		int k=indices[i],	// ������ ������� � ������� ������
			j=i%3;			// ����� ������� � ������������
		assert (k>=0 && k<nvertices);

		MVert[j]=k;				// ������ j-�� ������� �������� ������������ � ������� ������
		RVert[j]=XMLoadFloat3(&vertices[k].Pos);	// �� ������� � ������������

		if (j==2)					// ����� ������������
		{
			DirectX::XMVECTOR Norm = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(RVert[1]-RVert[0], RVert[2]-RVert[0]));
			for (j=0; j<3; j++)		// ����� �������� �� 3 �������� ������������
			{
				k = MVert[j];
				DirectX::XMFLOAT3 * P = &vertices[k].Norm;
				DirectX::XMStoreFloat3(P, Norm+ DirectX::XMLoadFloat3(P));		// ��������������� � Vert[m].Norm
			}
		}
	}
	for (i=0; i<nvertices; i++)			// ���������� �������� � ��������
	{
		DirectX::XMFLOAT3 * P = &vertices[i].Norm;
		XMStoreFloat3( P, DirectX::XMVector3Normalize( XMLoadFloat3( P ) ) );
	}

#if DEBUG_NORMALS == 1
	{
	XMFLOAT3 Max = Zero;
	for (i=0; i<nvertices; i++)
	{
		deb<<DebOld[i].x<<" "<<DebOld[i].y<<" "<<DebOld[i].z<<"    ";
		deb<<vertices[i].Norm.x<<" "<<vertices[i].Norm.y<<" "<<vertices[i].Norm.z<<"    ";
		float dx = vertices[i].Norm.x-DebOld[i].x;
		float dy = vertices[i].Norm.y-DebOld[i].y;
		float dz = vertices[i].Norm.z-DebOld[i].z;
		deb<<dx<<" "<<dy<<" "<<dz<<endl;
		Max.x = fabs(dx)>fabs(Max.x)? dx : Max.x;
		Max.y = fabs(dy)>fabs(Max.y)? dy : Max.y;
		Max.z = fabs(dz)>fabs(Max.z)? dz : Max.z;
	}
	deb<<"Max. difference: "<<Max.x<<" "<<Max.y<<" "<<Max.z<<endl;
	}
	deb.close();
	delete DebOld;
#endif

}

// ���������� ���������� � ���������� �������
void Mesh::CreateVerticesIndicesBuffers()
{
		HRESULT hr;

		if (vertexbuffer) vertexbuffer->Release();		// ������������ ������, ���� ��� ����� ���� ��������
		if (indexbuffer) indexbuffer->Release();

		hr=CreateBuffer(vertices, nvertices * sizeof(VERTEX), D3D11_BIND_VERTEX_BUFFER, &vertexbuffer);
		assert(!FAILED(hr)); 
		hr=CreateBuffer(indices, nindices * sizeof(WORD), D3D11_BIND_INDEX_BUFFER, &indexbuffer);
		assert(!FAILED(hr)); 
}

// ���������� ������� ���� ��� ������
void Mesh::SetWorld(DirectX::CXMMATRIX World)		// �� ����� ��������� ���� CXMMATRIX ����� ����������� ����� �������� ���� XMMATRIX
{
		XMStoreFloat4x4(&world, World);
}

// ���������� ���� pixelshader
void Mesh::SetPixelShader(ID3D11PixelShader *PixelShader)
{
	pixelshader = PixelShader;
}

//---------------------------------------------------------------------------
// ������ ��� ���������� �������� ���������� ������ Mesh
//---------------------------------------------------------------------------
VERTEX * Mesh::GetVertices()
{
	return vertices;
}

WORD * Mesh::GetIndices()
{
	return indices;
}

int Mesh::GetNVertices()
{
	return nvertices;
}

int Mesh::GetNIndices()
{
	return nindices;
}

ID3D11Buffer * Mesh::GetVertexBuffer()
{
	return vertexbuffer;
}

ID3D11Buffer * Mesh::GetIndexBuffer()
{
	return indexbuffer;
}

DirectX::XMMATRIX Mesh::GetWorld()
{
	return XMLoadFloat4x4( &world );
}

ID3D11PixelShader * Mesh::GetPixelShader()
{
	return pixelshader;
}


//---------------------------------------------------------------------------
// ������ ��� ���������� �� ������ �������� ������ Mesh
//---------------------------------------------------------------------------

// ���������� ������ ��� ��������� ������� ����, ������� � ����������� �������
void Mesh::Draw_0()
{
		Meshes::g_pImmediateContext->DrawIndexed(nindices, 0, 0); 
}

// ���������� ������ � ���������� ������� ����, ������������, ���������� � ���������� �������, ����������� �������
void Mesh::Draw()
	{
		*(Meshes::g_pWorld) = XMMatrixTranspose( XMLoadFloat4x4( &world ) );
		SetBuffers(vertexbuffer, indexbuffer, pixelshader);
		Meshes::g_pImmediateContext->DrawIndexed(nindices, 0, 0); 
	}

// ������� ���������� ������� Draw(), �� ������� ���� = (���� world) * (�������� World) , ����� ������ �� ���
void Mesh::Draw(CXMMATRIX World)
	{
		*(Meshes::g_pWorld) = XMMatrixTranspose( XMLoadFloat4x4( &world ) * World );	
		SetBuffers(vertexbuffer, indexbuffer, pixelshader);
		Meshes::g_pImmediateContext->DrawIndexed(nindices, 0, 0); 
	}


//---------------------------------------------------------------------------
// �������� ������ �� .obj �����
//---------------------------------------------------------------------------
void Mesh::LoadFromObj(const char * FileName)
{
//---------------------------------------------------------------------------
// ������ ������������ ���������� ������ � ��������. ��� ��������
// ����� ��������� ���� ��������� ��������� ������� ����       
//---------------------------------------------------------------------------
const int buffermax=16384;
//---------------------------------------------------------------------------

	VERTEX * bufferVertices = new VERTEX[buffermax];		// ������� ��� ���������� �������� ������ � ��������
	WORD * bufferIndices = new WORD[buffermax];
    int verticesI=0;													// ������� � ���� ��������
    int indicesI=0;

    //�������� ���������� ��������� �������� ������, ��� ������ ������ �����
    //��������� � ���������� ������ ��������� ��� ������ � ��������� � ��������� ������
    XMFLOAT3 * Positions = new XMFLOAT3[buffermax];
    XMFLOAT2 * TexCoords = new XMFLOAT2[buffermax];
    XMFLOAT3 * Normals = new XMFLOAT3[buffermax];
	    // ������� ��� ��������
    int PositionsI=0;
    int TexCoordsI=0;
    int NormalsI=0;

    // ���� ������ �� �����
    //char fname[256];
    //strcpy(fname,Edit1->Text.c_str());
    WCHAR strCommand[256] = {0};
    wifstream InFile( FileName );

    if( !InFile ) 
		return;

    for(; ; )
    {
        InFile >> strCommand;
        if( !InFile )
            break;

        if( 0 == wcscmp( strCommand, L"#" ) )
        {
            // �����������
        }
        else if( 0 == wcscmp( strCommand, L"v" ) )
        {
            // ����������
            float x, y, z;
            InFile >> x >> y >> z; 
			//float c=0.05f;
            Positions[PositionsI++]=XMFLOAT3(x,y,z);
        }
        else if( 0 == wcscmp( strCommand, L"vt" ) )
        {
            // ���������� ����������
            float u, v;
            InFile >> u >> v;
			TexCoords[TexCoordsI++]=XMFLOAT2(u,v);
        }
        else if( 0 == wcscmp( strCommand, L"vn" ) )
        {
            // �������
            float x, y, z;
            InFile >> x >> y >> z;
            Normals[NormalsI++]=XMFLOAT3(x,y,z);
        }
        else if( 0 == wcscmp( strCommand, L"f" ) )
        {
            // Face
            UINT iPosition, iTexCoord, iNormal;
            VERTEX vertex;

            for( UINT iFace = 0; iFace < 3; iFace++ )
            {
                ZeroMemory( &vertex, sizeof( VERTEX ) );

                // OBJ ������ ���������� ������� � ��������� �� 1
                InFile >> iPosition;
				vertex.Pos = Positions[ iPosition - 1 ];

                if( '/' == InFile.peek() )
                {
                    InFile.ignore();

                    if( '/' != InFile.peek() )
                    {
                        // �������� ���������� ����������
                        InFile >> iTexCoord;
						vertex.TexPos = XMFLOAT3( TexCoords[ iTexCoord - 1 ].x, TexCoords[ iTexCoord - 1 ].y, 0 );
                    }

                    if( '/' == InFile.peek() )
                    {
                        InFile.ignore();

                        // �������� �������
                        InFile >> iNormal;
						vertex.Norm = Normals[ iNormal - 1 ];
                    }
                }

                //��������� ������� � ������
				int index=-1;
					//����� ������������ �������
				for (int i=0; i<verticesI; i++)
					if ( memcmp( &bufferVertices[i], &vertex, sizeof(VERTEX) ) == 0 ) 
						index = i;
					//����������
				if (index<0)
				{
					bufferVertices[verticesI++]=vertex; 
					index = verticesI-1; 
				}
				
				bufferIndices[indicesI++]=index;
            }
		}
    }
    InFile.close();

 	SetVerticesIndices(bufferVertices, verticesI, bufferIndices, indicesI, 0);

   //������� ��������� ��������
    delete Positions;
    delete TexCoords;
    delete Normals;
	delete bufferVertices;
	delete bufferIndices;
}


//********************************************************************************************************
// ������ ������ Instance
//********************************************************************************************************

Instance::Instance()
{
	mesh = NULL;
	XMStoreFloat4x4(&world, XMMatrixIdentity());
}

Instance::Instance(Mesh * M)
{
	mesh = M;
	XMStoreFloat4x4(&world, XMMatrixIdentity());
}

Instance::Instance(Mesh * M, CXMMATRIX World)
{
	mesh = M;
	XMStoreFloat4x4(&world, World);
}

Instance::~Instance()		// ���������� ������
{
}

void Instance::SetWorld(CXMMATRIX World)
{
	XMStoreFloat4x4(&world, World);
}

XMMATRIX Instance::GetWorld()
{
	return XMLoadFloat4x4(&world);
}

//---------------------------------------------------------------------------
// ���������� ��������� �� ������
//---------------------------------------------------------------------------

void Instance::Draw_0()		// ������� ���� �� ���������� (������������ ������� ��������); 
{							// �� ��������������� ������ � ���������� ������
	mesh->Draw_0();
}
						
void Instance::Draw()		// ������� ���� = (���� world ����) * (���� world ���������) , ����� ������ �� ���; 
{							// ��������������� ������ � ���������� ������
	mesh->Draw( XMLoadFloat4x4( &world ) );
}



//********************************************************************************************************
// ������ ������ �������, ����� Mesh � Instance
//********************************************************************************************************

//---------------------------------------------------------------------------
// ������� ������ (h - ������, r - ������ ���������, ny - ����� ������ �� ��������� (������), nFi - ����� ������ �� �����
//---------------------------------------------------------------------------
Cone::Cone(double h, double r, int ny, int nFi) : Mesh()
{
	nvertices = ny*nFi+2;		// ny ������, �� ������ �� nFi ������, + ������� � ������ (� ������ ���������) �������
	vertices = new VERTEX[ nvertices ];
	nindices = 6*ny*nFi;		// ����� ������������� = nFi(��� ������� ������) + nFi(��� ���������) + 2*(ny-1)*nFi(�������) = 2*ny*nFi
	indices = new WORD[ nindices ];

	int i, j, k, k1, p, p1;
	VERTEX * V = vertices;
	WORD * I = indices;

	ZeroMemory(V, nvertices*sizeof(VERTEX) );
	V->Pos = XMFLOAT3(0, h, 0);		// ������� ������
	V->Norm = XMFLOAT3(0, 1, 0);
	k=0;
	for (i=1; i<=ny; i++)
		for (j=0; j<nFi; j++)
		{
			double fi = j * XM_2PI / nFi;
			double ri = r * (double)i / ny; 
			double d = h * (double)i / ny;
			(++V)->Pos = XMFLOAT3( ri * cos(fi), h - d, ri * sin(fi) );	
					// ���������� ��������
			double ConeAngle = atan(r/h);
			V->Norm = XMFLOAT3( cos(ConeAngle) * cos(fi), sin(ConeAngle), cos(ConeAngle) * sin(fi));

			//V->TexPos =			// ������� ������� � �������� (���� �����)
			//V->TexNormPos =

			++k;						// ������ ������� �������
			k1 = j < nFi-1 ?  k+1 :  k-j;	// ������ ��������� ������� �������� �����
			if (i==1)		// ������ �� ������
			{
				*I++ = 0;
				*I++ = k1;		// ����� �� ������� �������, ����� �������
				*I++ = k;
			}
			else 
			{
				p = k - nFi;	// ������ ��������������� ������� ����������� �����
				p1 = j < nFi-1 ?  p+1 :  p-j;	// ������ ��������� ������� ����������� �����
				*I++ = p;
				*I++ = k1;
				*I++ = k;
				*I++ = p;
				*I++ = p1;
				*I++ = k1;
			}
		}

  // ��������� ������
	(++V)->Pos = XMFLOAT3(0, 0, 0);	// ����� ��������� ������
	V->Norm = XMFLOAT3(0, -1, 0);
	++k;						// ������ ������� ������� (������ ���������)
	for (j=0; j<nFi; j++)
	{
		++p;	// ������ ��������� ������� ����������� �����
		p1 = j<nFi-1? p+1 : p-j;	// ������ ��������� ������� ����������� �����
		*I++ = p;
		*I++ = p1;		// ����� �� ������� �������, ����� �������
		*I++ = k;
	}
	// ��������, �� ������� �� �� �� �����
	assert(V==vertices+nvertices-1);	// -1 ������, ��� ����� ��������� ������� �� ������ V++
	assert(I==indices+nindices);

 	//CalculateNormals();		// ���� ������� �� ��������� �� �����; ������������� ������� ������, ��� ��������� ������

	CreateVerticesIndicesBuffers();		// ���������� ���������� � ���������� �������
}


//---------------------------------------------------------------------------
// �������� �������������� � �������
//---------------------------------------------------------------------------
void TextMesh::Print(char * Text)
{
	Release();		// ������������ ������, ���� ��� ����� ���� ��������

	int N = strlen(Text);
	nvertices = N*4;
	nindices = N*6;
	vertices = new VERTEX[nvertices];
	indices = new WORD[nindices];

	VERTEX * V = vertices;
	ZeroMemory(V, nvertices*sizeof(VERTEX) );
	WORD * I = indices;
	XMVECTOR pos = XMVectorSet(0,1,0,0);		// ������� ����� ���� ������
	XMVECTOR step = XMVectorSet(1,0,0,0);		// ������ �����
	XMVECTOR stepY = XMVectorSet(0,1,0,0);		// ������ ����� 

	for (int i=0; i<N; i++)
	{
		unsigned char c = Text[i];		// unsigned, ����� ����� �������� ������� ����� (���� > 127)
		float x1 = c%32 / 32.0;
		float x2 = x1 + 1/32.0;
		float y1 = c/32 / 8.0;
		float y2 = y1 + 1/8.0;

		XMStoreFloat3(&V->Pos, pos);
		V->TexPos = XMFLOAT3(x1,y1,0);
		V++;

		XMStoreFloat3(&V->Pos, pos + step);
		V->TexPos = XMFLOAT3(x2,y1,0);
		V++;
		
		XMStoreFloat3(&V->Pos, pos - stepY);
		V->TexPos = XMFLOAT3(x1,y2,0);
		V++;

		XMStoreFloat3(&V->Pos, pos + step - stepY);
		V->TexPos = XMFLOAT3(x2,y2,0);
		V++;

		pos += step;

		int k = i*4;
		*I++ = k;
		*I++ = k+1;
		*I++ = k+2;
		*I++ = k+2;
		*I++ = k+1;
		*I++ = k+3;
	}

	CalculateNormals();
	CreateVerticesIndicesBuffers();
}


//---------------------------------------------------------------------------
// ��������� ������ �� �����
//---------------------------------------------------------------------------

void TextMesh::PrintCone(char * Text, double h1, double r1, double h2, double r2, double fi0)
{
	Release();		// ������������ ������, ���� ��� ����� ���� ��������

	int N = strlen(Text);
	nvertices = N*4;
	nindices = N*6;
	vertices = new VERTEX[nvertices];
	indices = new WORD[nindices];

	VERTEX * V = vertices;
	ZeroMemory(V, nvertices*sizeof(VERTEX) );
	WORD * I = indices;

	double SymbolSize = sqrt( (h1-h2)*(h1-h2) + (r1-r2)*(r1-r2) );	// ������ �������
	double dfi = SymbolSize / ( (r1+r2)/2 );			// ������� ������ �������
	double ConeAngle = atan( (r2-r1)/(h1-h2) );

	double fi = fi0;

	for (int i=0; i<N; i++)
	{
		unsigned char c = Text[i];
		float x1 = c%32 / 32.0;
		float x2 = x1 + 1/32.0;
		float y1 = c/32 / 8.0;
		float y2 = y1 + 1/8.0;

		V->Pos = XMFLOAT3( r1*cos(fi), h1, r1*sin(fi) );
		V->TexPos = XMFLOAT3(x1,y1,0);
		V->Norm = XMFLOAT3( cos(ConeAngle) * cos(fi), sin(ConeAngle), cos(ConeAngle) * sin(fi));
		V++;

		V->Pos = XMFLOAT3( r1*cos(fi+dfi), h1, r1*sin(fi+dfi) );
		V->TexPos = XMFLOAT3(x2,y1,0);
		V->Norm = XMFLOAT3( cos(ConeAngle) * cos(fi+dfi), sin(ConeAngle), cos(ConeAngle) * sin(fi+dfi));
		V++;
		
		V->Pos = XMFLOAT3( r2*cos(fi), h2, r2*sin(fi) );
		V->TexPos = XMFLOAT3(x1,y2,0);
		V->Norm = XMFLOAT3( cos(ConeAngle) * cos(fi), sin(ConeAngle), cos(ConeAngle) * sin(fi));
		V++;

		V->Pos = XMFLOAT3( r2*cos(fi+dfi), h2, r2*sin(fi+dfi) );
		V->TexPos = XMFLOAT3(x2,y2,0);
		V->Norm = XMFLOAT3( cos(ConeAngle) * cos(fi+dfi), sin(ConeAngle), cos(ConeAngle) * sin(fi+dfi));
		V++;

		fi += dfi;

		int k = i*4;
		*I++ = k;
		*I++ = k+1;
		*I++ = k+2;
		*I++ = k+2;
		*I++ = k+1;
		*I++ = k+3;
	}

	//CalculateNormals(); // ���� ������� �� ��������� �� �����; ������������� ������� ������, ��� ��������� ������
	CreateVerticesIndicesBuffers();
}


void TextMesh::PrintCone(char * Text, Cone & C, double hRel1, double hRel2, double fi0, double Magnify)
{
	int NVert = C.GetNVertices();
	VERTEX * Vert = C.GetVertices();
	double h = Vert[0].Pos.y;			// Vert[0] - ������� ����� ������ (������� ������ � �������������� ������)
	XMFLOAT3 pos = Vert[NVert-2].Pos;	// Vert[NVert-2] - ���� �� ������ ��������� ������
	double r = sqrt(pos.x*pos.x+pos.z*pos.z);

		// ���������� ������� ������, ����� �� �� ���������� �������
	r *= Magnify;

	PrintCone(Text, h*hRel1, r*(1-hRel1), h*hRel2, r*(1-hRel2), fi0);

	SetWorld( C.GetWorld() );
}

//********************************************************************************************************
// ������� �������� ������ ������, �������� (shaders) � �������� ������� ������ (input layout)
//********************************************************************************************************

//--------------------------------------------------------------------------------------
// ��������������� ������� ��� �������� ������� �� ������� �������
//--------------------------------------------------------------------------------------
HRESULT CreateBuffer(void* source, int size, UINT BindFlags, ID3D11Buffer** destination)
{
	// ���������� (��� �������� ��������) ������� �� namespace Meshes
	ID3D11Device* Device = Meshes::g_pd3dDevice;

	D3D11_BUFFER_DESC bd;	// ���������, ����������� ����������� �����
	ZeroMemory( &bd, sizeof(bd) );				// ������� ��
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = size;	// ������ ������
    bd.BindFlags = BindFlags;	// ��� ������ 
	bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;	// ���������, ���������� ������ ������
	ZeroMemory( &InitData, sizeof(InitData) );	// ������� ��
    InitData.pSysMem = source;				// ��������� �� ���� ������

	// ����� ������ CreateBuffer ������� Device ������� ������ ������
    return Device->CreateBuffer( &bd, &InitData, destination );
}

//--------------------------------------------------------------------------------------
// ��������������� ������� ��� ���������� �������� � D3DX11 (�� ���������� Meshes.h)
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
    HRESULT hr = S_OK;
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
    ID3DBlob* pErrorBlob;
	hr = D3DCompileFromFile(
		szFileName,
		NULL,
		NULL,
		szEntryPoint,
		szShaderModel,
		D3DCOMPILE_DEBUG,
		0,
		ppBlobOut,
		NULL);
 

    return S_OK;
}

//--------------------------------------------------------------------------------------
// ��������������� ������� ��� ���������� � �������� ���������� �������� �� ������� �������, 
// � ����� �������� ������� ������
//--------------------------------------------------------------------------------------
HRESULT CreateVertexShader(WCHAR * szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, 
	ID3D11VertexShader** ppVertexShader, ID3D11InputLayout** ppVertexLayout)
{
	// ���������� (��� �������� ��������) ������� �� namespace Meshes
	ID3D11Device* Device = Meshes::g_pd3dDevice;

	ID3DBlob* pBlob = NULL; // ��������������� ������ - ������ ����� � ����������� ������

	// ���������� ����������� ������� �� �����
	HRESULT hr = CompileShaderFromFile(szFileName, szEntryPoint, szShaderModel, &pBlob);
    if (FAILED(hr))
    {
        MessageBox( NULL, L"���������� �������������� ���� FX: ����������, ��������� ������ ��������� �� �����, ���������� ���� FX.",
			L"������", MB_OK );
        return hr;
    }

	// �������� ����������� ������� 
	hr = Device->CreateVertexShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, ppVertexShader );
    if (FAILED(hr))
	{	
		pBlob->Release();
        return hr;
	}

    // �������� ������� ������	(layout � numElements ���������� � ����� Meshes.h)
	hr = Device->CreateInputLayout( layout, numElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), ppVertexLayout );
	pBlob->Release();
	if (FAILED(hr)) return hr;

	return S_OK;
}

//--------------------------------------------------------------------------------------
// ��������������� ������� ��� ���������� � �������� ���������� �������� �� ������� �������
//--------------------------------------------------------------------------------------
HRESULT CreatePixelShader(WCHAR * szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel,
	ID3D11PixelShader** ppPixelShader)
{
	// ���������� (��� �������� ��������) ������� �� namespace Meshes
	ID3D11Device* Device = Meshes::g_pd3dDevice;

	ID3DBlob* pBlob = NULL; // ��������������� ������ - ������ ����� � ����������� ������

	// ���������� ����������� ������� �� �����
	HRESULT hr = CompileShaderFromFile(szFileName, szEntryPoint, szShaderModel, &pBlob);
    if (FAILED(hr))
    {
        MessageBox( NULL, L"���������� �������������� ���� FX. ����������, ��������� ������ ��������� �� �����, ���������� ���� FX.",
			L"������", MB_OK );
        return hr;
    }

	// �������� ����������� ������� 
	hr = Device->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, ppPixelShader );
	if (FAILED(hr)) return hr;
	pBlob->Release();

	return S_OK;
}

//--------------------------------------------------------------------------------------
// ��������������� ������� ��������� ������� � ����������� ������� ��� ������ ��������� ������� 
//--------------------------------------------------------------------------------------
void SetBuffers(ID3D11Buffer* VertexBuffer, ID3D11Buffer* IndexBuffer, ID3D11PixelShader* PixelShader)
{
		// �������� (���������� ���������), ����� � ������� ������������ ������������ ������ ������� �� namespace Meshes
	ID3D11DeviceContext* Context = Meshes::g_pImmediateContext;
	ID3D11Buffer* pConstantBuffer = Meshes::g_pConstantBuffer;
	void* pcb = Meshes::P_cb;
	int BufferIndex = Meshes::ConstantBufferRegisterIndex;

		// ��������� ����������� �������
	Context->PSSetShader( PixelShader, NULL, 0 );

		// ��������� ������������ ������
	Context->UpdateSubresource( pConstantBuffer, 0, NULL, pcb, 0, 0 ); // ��������� cb � g_pConstantBuffer
	Context->VSSetConstantBuffers( BufferIndex, 1, &pConstantBuffer );	// ������ ����������� ����� ��� ���������� �������
	Context->PSSetConstantBuffers( BufferIndex, 1, &pConstantBuffer );	// ������ ����������� ����� ��� ����������� �������

		// ��������� ������ ������
 	UINT stride = sizeof( VERTEX );
    UINT offset = 0;
	Context->IASetVertexBuffers( 0, 1, &VertexBuffer, &stride, &offset );

		// ��������� ������ ��������
    Context->IASetIndexBuffer( IndexBuffer, DXGI_FORMAT_R16_UINT, 0 );

		// ��������� ������� ��������� ������ � ������
    Context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}



