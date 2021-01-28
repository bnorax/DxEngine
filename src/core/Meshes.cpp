#include "Meshes.h"
#include<fstream>

using namespace DirectX;
//--------------------------------------------------------------------------------------
// Глобальные переменные модуля (namespace Meshes)
// Им должны быть присвоены нужные ненулевые значения ДО вызова большинства функций и 
// методов модуля (включая конструкторы с параметрами)
//--------------------------------------------------------------------------------------
namespace Meshes
{
	ID3D11Device* g_pd3dDevice = NULL;					// Устройство (для создания объектов)
	ID3D11DeviceContext* g_pImmediateContext = NULL;	// Контекст (устройство рисования)

				// Следующие переменные используются только в функциях, меняющих матрицу мира
	ID3D11Buffer* g_pConstantBuffer = NULL;				// Константный буфер, обновляемый вместе с матрицей мира
	void* P_cb = NULL;				// Указатель на структуру, используемую для заполнения этого буфера
	int Size_cb = 0;				// Размер этой структуры в байтах 
	DirectX::XMMATRIX* g_pWorld = NULL;		// Указатель на матрицу мира, входящую в состав этой структуры
	int ConstantBufferRegisterIndex = 0;	// Индекс регистра b, куда записывается этот константный буфер
}

using namespace std;


//********************************************************************************************************
// Методы класса Mesh
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
// Конструкторы и деструкторы
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
// Задание вершин и треугольников меша по имеющимся массивам
//		Значения параметра SetNormal:
//		0 (по умолчанию) - не рассчитывать нормали, а скопировать значения, содержащиеся в массиве Vertices
//		1 - рассчитать нормали к вершинам
//		2 - рассчитать нормали к треугольникам; при этом число вершин может возрасти (по 3 вершины для каждого треугольника)
//---------------------------------------------------------------------------
void Mesh::SetVerticesIndices(const VERTEX *Vertices, int NVertices, const WORD *Indices, int NIndices, int SetNormal)
{
	Release();		// Освобождение памяти, если она ранее была выделена

	nindices = NIndices;
	indices = new WORD[NIndices];
	memcpy(indices, Indices, NIndices * sizeof(WORD) );

	if (SetNormal <= 1)			// Если нет нужды в создании добавочных вершин
	{
		nvertices = NVertices;
		vertices = new VERTEX[nvertices];
		memcpy(vertices, Vertices, NVertices * sizeof(VERTEX) );
	}
	else						// Создаем по 3 вершины для каждого треугольника
	{
		nvertices = NIndices;
		vertices = new VERTEX[nvertices];
		for (int i=0; i<NIndices; i++)	// номер в массиве индексов	(треугольников)
		{
			int k=Indices[i];	// номер вершины в исходном массиве
			assert (k>=0 && k<NVertices);

			vertices[i] = Vertices[k];
			indices[i] = i;
		}
	}

	if (SetNormal >= 1)			// Вычисление нормалей заново
		CalculateNormals();

	CreateVerticesIndicesBuffers();	// Заполнение буферов
}

//---------------------------------------------------------------------------
// Расчет нормалей
//---------------------------------------------------------------------------
void Mesh::CalculateNormals()
{
	DirectX::XMVECTOR RVert[3];		// позиции вершин треугольника
	int MVert[3];			// индексы вершин треугольника
	int i;

// Отладочные операторы, позволяющие сравнить нормали, рассчитанные по другой формуле, с нормалями, рассчитанными по соседним вершинам
// По умолчанию они выключены (макроконстанта DEBUG_NORMALS имеет значение 0, см. ниже) 
// Для их включения замените чуть ниже в директиве #define DEBUG_NORMALS значение 0 на 1
// а также в начале программы раскомментируйте (или добавьте)  #include<fstream>  и  using namespace std;

#define DEBUG_NORMALS 0

#if DEBUG_NORMALS == 1
	ofstream deb("DebugNormals.txt");
	deb<<"Old normals (x,y,z)     Calculated normals (x,y,z)   Difference"<<endl;
	XMFLOAT3 *DebOld = new XMFLOAT3[nvertices];
	for (i=0; i<nvertices; i++)			// Запоминаем старые нормали 
		DebOld[i] = vertices[i].Norm;
#endif

	DirectX::XMFLOAT3 Zero = DirectX::XMFLOAT3(0,0,0);
	for (i=0; i<nvertices; i++)			// Обнуляем нормали для последующего приплюсовывания
		vertices[i].Norm = Zero;		

	for (i=0; i<nindices; i++)	// номер в массиве индексов	(треугольников)
	{
		int k=indices[i],	// индекс вершины в массиве вершин
			j=i%3;			// номер вершины в треугольнике
		assert (k>=0 && k<nvertices);

		MVert[j]=k;				// индекс j-ой вершины текущего треугольника в массиве вершин
		RVert[j]=XMLoadFloat3(&vertices[k].Pos);	// ее позиция в пространстве

		if (j==2)					// конец треугольника
		{
			DirectX::XMVECTOR Norm = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(RVert[1]-RVert[0], RVert[2]-RVert[0]));
			for (j=0; j<3; j++)		// опять проходим по 3 вершинам треугольника
			{
				k = MVert[j];
				DirectX::XMFLOAT3 * P = &vertices[k].Norm;
				DirectX::XMStoreFloat3(P, Norm+ DirectX::XMLoadFloat3(P));		// приплюсовывание к Vert[m].Norm
			}
		}
	}
	for (i=0; i<nvertices; i++)			// Нормировка нормалей к вершинам
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

// Заполнение вершинного и индексного буферов
void Mesh::CreateVerticesIndicesBuffers()
{
		HRESULT hr;

		if (vertexbuffer) vertexbuffer->Release();		// Освобождение памяти, если она ранее была выделена
		if (indexbuffer) indexbuffer->Release();

		hr=CreateBuffer(vertices, nvertices * sizeof(VERTEX), D3D11_BIND_VERTEX_BUFFER, &vertexbuffer);
		assert(!FAILED(hr)); 
		hr=CreateBuffer(indices, nindices * sizeof(WORD), D3D11_BIND_INDEX_BUFFER, &indexbuffer);
		assert(!FAILED(hr)); 
}

// Заполнение матрицы мира для фигуры
void Mesh::SetWorld(DirectX::CXMMATRIX World)		// на место параметра типа CXMMATRIX можно подставлять любое значение типа XMMATRIX
{
		XMStoreFloat4x4(&world, World);
}

// Заполнение поля pixelshader
void Mesh::SetPixelShader(ID3D11PixelShader *PixelShader)
{
	pixelshader = PixelShader;
}

//---------------------------------------------------------------------------
// Методы для извлечения значений параметров класса Mesh
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
// Методы для прорисовки на экране объектов класса Mesh
//---------------------------------------------------------------------------

// Прорисовка фигуры без установки матрицы мира, буферов и пиксельного шейдера
void Mesh::Draw_0()
{
		Meshes::g_pImmediateContext->DrawIndexed(nindices, 0, 0); 
}

// Прорисовка фигуры с установкой матрицы мира, константного, вершинного и индексного буферов, пиксельного шейдера
void Mesh::Draw()
	{
		*(Meshes::g_pWorld) = XMMatrixTranspose( XMLoadFloat4x4( &world ) );
		SetBuffers(vertexbuffer, indexbuffer, pixelshader);
		Meshes::g_pImmediateContext->DrawIndexed(nindices, 0, 0); 
	}

// Подобна предыдущей функции Draw(), но матрица мира = (поле world) * (параметр World) , чтобы учесть их оба
void Mesh::Draw(CXMMATRIX World)
	{
		*(Meshes::g_pWorld) = XMMatrixTranspose( XMLoadFloat4x4( &world ) * World );	
		SetBuffers(vertexbuffer, indexbuffer, pixelshader);
		Meshes::g_pImmediateContext->DrawIndexed(nindices, 0, 0); 
	}


//---------------------------------------------------------------------------
// Загрузка фигуры из .obj файла
//---------------------------------------------------------------------------
void Mesh::LoadFromObj(const char * FileName)
{
//---------------------------------------------------------------------------
// Задаем максимальное количество вершин и индексов. Это значение
// можно увеличить если требуется загружать большие меши       
//---------------------------------------------------------------------------
const int buffermax=16384;
//---------------------------------------------------------------------------

	VERTEX * bufferVertices = new VERTEX[buffermax];		// Массивы для временного хранения вершин и индексов
	WORD * bufferIndices = new WORD[buffermax];
    int verticesI=0;													// Индексы в этих массивах
    int indicesI=0;

    //Создание временного хранилища входящих данных, как только данные будут
    //загружены в подходящий формат скопируем эти данные в вершинный и индексный буферы
    XMFLOAT3 * Positions = new XMFLOAT3[buffermax];
    XMFLOAT2 * TexCoords = new XMFLOAT2[buffermax];
    XMFLOAT3 * Normals = new XMFLOAT3[buffermax];
	    // Индексы для массивов
    int PositionsI=0;
    int TexCoordsI=0;
    int NormalsI=0;

    // Ввод данных из файла
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
            // Комментарий
        }
        else if( 0 == wcscmp( strCommand, L"v" ) )
        {
            // Координаты
            float x, y, z;
            InFile >> x >> y >> z; 
			//float c=0.05f;
            Positions[PositionsI++]=XMFLOAT3(x,y,z);
        }
        else if( 0 == wcscmp( strCommand, L"vt" ) )
        {
            // Текстурные координаты
            float u, v;
            InFile >> u >> v;
			TexCoords[TexCoordsI++]=XMFLOAT2(u,v);
        }
        else if( 0 == wcscmp( strCommand, L"vn" ) )
        {
            // Нормали
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

                // OBJ формат использует массивы с индексами от 1
                InFile >> iPosition;
				vertex.Pos = Positions[ iPosition - 1 ];

                if( '/' == InFile.peek() )
                {
                    InFile.ignore();

                    if( '/' != InFile.peek() )
                    {
                        // Опционно текстурные координаты
                        InFile >> iTexCoord;
						vertex.TexPos = XMFLOAT3( TexCoords[ iTexCoord - 1 ].x, TexCoords[ iTexCoord - 1 ].y, 0 );
                    }

                    if( '/' == InFile.peek() )
                    {
                        InFile.ignore();

                        // Опционно нормали
                        InFile >> iNormal;
						vertex.Norm = Normals[ iNormal - 1 ];
                    }
                }

                //Добавляем вершину и индекс
				int index=-1;
					//Поиск существующей вершины
				for (int i=0; i<verticesI; i++)
					if ( memcmp( &bufferVertices[i], &vertex, sizeof(VERTEX) ) == 0 ) 
						index = i;
					//Добавление
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

   //Очистка временных массивов
    delete Positions;
    delete TexCoords;
    delete Normals;
	delete bufferVertices;
	delete bufferIndices;
}


//********************************************************************************************************
// Методы класса Instance
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

Instance::~Instance()		// деструктор пустой
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
// Прорисовка инстанции на экране
//---------------------------------------------------------------------------

void Instance::Draw_0()		// Матрица мира не изменяется (используется текущее значение); 
{							// не устанавливаются буфера и пиксельный шейдер
	mesh->Draw_0();
}
						
void Instance::Draw()		// Матрица мира = (поле world меша) * (поле world инстанции) , чтобы учесть их оба; 
{							// устанавливаются буфера и пиксельный шейдер
	mesh->Draw( XMLoadFloat4x4( &world ) );
}



//********************************************************************************************************
// Методы других классов, кроме Mesh и Instance
//********************************************************************************************************

//---------------------------------------------------------------------------
// Задание конуса (h - высота, r - радиус основания, ny - число вершин по вертикали (кругов), nFi - число вершин по кругу
//---------------------------------------------------------------------------
Cone::Cone(double h, double r, int ny, int nFi) : Mesh()
{
	nvertices = ny*nFi+2;		// ny кругов, на каждом по nFi вершин, + верхняя и нижняя (в центре основания) вершины
	vertices = new VERTEX[ nvertices ];
	nindices = 6*ny*nFi;		// Число треугольников = nFi(для вершины конуса) + nFi(для основания) + 2*(ny-1)*nFi(боковые) = 2*ny*nFi
	indices = new WORD[ nindices ];

	int i, j, k, k1, p, p1;
	VERTEX * V = vertices;
	WORD * I = indices;

	ZeroMemory(V, nvertices*sizeof(VERTEX) );
	V->Pos = XMFLOAT3(0, h, 0);		// Вершина конуса
	V->Norm = XMFLOAT3(0, 1, 0);
	k=0;
	for (i=1; i<=ny; i++)
		for (j=0; j<nFi; j++)
		{
			double fi = j * XM_2PI / nFi;
			double ri = r * (double)i / ny; 
			double d = h * (double)i / ny;
			(++V)->Pos = XMFLOAT3( ri * cos(fi), h - d, ri * sin(fi) );	
					// Вычисление нормалей
			double ConeAngle = atan(r/h);
			V->Norm = XMFLOAT3( cos(ConeAngle) * cos(fi), sin(ConeAngle), cos(ConeAngle) * sin(fi));

			//V->TexPos =			// Задание позиций в текстуре (если нужно)
			//V->TexNormPos =

			++k;						// Индекс текущей вершины
			k1 = j < nFi-1 ?  k+1 :  k-j;	// Индекс следующей вершины текущего круга
			if (i==1)		// Первый из кругов
			{
				*I++ = 0;
				*I++ = k1;		// Обход по часовой стрелке, глядя снаружи
				*I++ = k;
			}
			else 
			{
				p = k - nFi;	// Индекс соответствующей вершины предыдущего круга
				p1 = j < nFi-1 ?  p+1 :  p-j;	// Индекс следующей вершины предыдущего круга
				*I++ = p;
				*I++ = k1;
				*I++ = k;
				*I++ = p;
				*I++ = p1;
				*I++ = k1;
			}
		}

  // Основание конуса
	(++V)->Pos = XMFLOAT3(0, 0, 0);	// Центр основания конуса
	V->Norm = XMFLOAT3(0, -1, 0);
	++k;						// Индекс текущей вершины (центра основания)
	for (j=0; j<nFi; j++)
	{
		++p;	// Индекс очередной вершины предыдущего круга
		p1 = j<nFi-1? p+1 : p-j;	// Индекс следующей вершины предыдущего круга
		*I++ = p;
		*I++ = p1;		// Обход по часовой стрелке, глядя снаружи
		*I++ = k;
	}
	// Проверка, не сбились ли мы со счета
	assert(V==vertices+nvertices-1);	// -1 оттого, что после последней вершины не делали V++
	assert(I==indices+nindices);

 	//CalculateNormals();		// если нормали не вычислены до этого; аналитическая формула точнее, чем численный расчет

	CreateVerticesIndicesBuffers();		// Заполнение вершинного и индексного буферов
}


//---------------------------------------------------------------------------
// Создание прямоугольника с текстом
//---------------------------------------------------------------------------
void TextMesh::Print(char * Text)
{
	Release();		// Освобождение памяти, если она ранее была выделена

	int N = strlen(Text);
	nvertices = N*4;
	nindices = N*6;
	vertices = new VERTEX[nvertices];
	indices = new WORD[nindices];

	VERTEX * V = vertices;
	ZeroMemory(V, nvertices*sizeof(VERTEX) );
	WORD * I = indices;
	XMVECTOR pos = XMVectorSet(0,1,0,0);		// Верхний левый угол текста
	XMVECTOR step = XMVectorSet(1,0,0,0);		// Ширина буквы
	XMVECTOR stepY = XMVectorSet(0,1,0,0);		// Высота буквы 

	for (int i=0; i<N; i++)
	{
		unsigned char c = Text[i];		// unsigned, чтобы верно выводило русские буквы (коды > 127)
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
// Нанесение текста на конус
//---------------------------------------------------------------------------

void TextMesh::PrintCone(char * Text, double h1, double r1, double h2, double r2, double fi0)
{
	Release();		// Освобождение памяти, если она ранее была выделена

	int N = strlen(Text);
	nvertices = N*4;
	nindices = N*6;
	vertices = new VERTEX[nvertices];
	indices = new WORD[nindices];

	VERTEX * V = vertices;
	ZeroMemory(V, nvertices*sizeof(VERTEX) );
	WORD * I = indices;

	double SymbolSize = sqrt( (h1-h2)*(h1-h2) + (r1-r2)*(r1-r2) );	// Размер символа
	double dfi = SymbolSize / ( (r1+r2)/2 );			// Угловая ширина символа
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

	//CalculateNormals(); // если нормали не вычислены до этого; аналитическая формула точнее, чем численный расчет
	CreateVerticesIndicesBuffers();
}


void TextMesh::PrintCone(char * Text, Cone & C, double hRel1, double hRel2, double fi0, double Magnify)
{
	int NVert = C.GetNVertices();
	VERTEX * Vert = C.GetVertices();
	double h = Vert[0].Pos.y;			// Vert[0] - верхняя точка конуса (вершина конуса в геометрическом смысле)
	XMFLOAT3 pos = Vert[NVert-2].Pos;	// Vert[NVert-2] - одна из вершин основания конуса
	double r = sqrt(pos.x*pos.x+pos.z*pos.z);

		// Увеличение радиуса текста, чтобы он не поглотился конусом
	r *= Magnify;

	PrintCone(Text, h*hRel1, r*(1-hRel1), h*hRel2, r*(1-hRel2), fi0);

	SetWorld( C.GetWorld() );
}

//********************************************************************************************************
// Функции создания буфера вершин, шейдеров (shaders) и описания формата вершин (input layout)
//********************************************************************************************************

//--------------------------------------------------------------------------------------
// Вспомогательная функция для создания буферов по единому образцу
//--------------------------------------------------------------------------------------
HRESULT CreateBuffer(void* source, int size, UINT BindFlags, ID3D11Buffer** destination)
{
	// Устройство (для создания объектов) берется из namespace Meshes
	ID3D11Device* Device = Meshes::g_pd3dDevice;

	D3D11_BUFFER_DESC bd;	// Структура, описывающая создаваемый буфер
	ZeroMemory( &bd, sizeof(bd) );				// очищаем ее
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = size;	// размер буфера
    bd.BindFlags = BindFlags;	// тип буфера 
	bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;	// Структура, содержащая данные буфера
	ZeroMemory( &InitData, sizeof(InitData) );	// очищаем ее
    InitData.pSysMem = source;				// указатель на наши данные

	// Вызов метода CreateBuffer объекта Device создаст объект буфера
    return Device->CreateBuffer( &bd, &InitData, destination );
}

//--------------------------------------------------------------------------------------
// Вспомогательная функция для компиляции шейдеров в D3DX11 (не использует Meshes.h)
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
// Вспомогательная функция для компиляции и создания вертексных шейдеров по единому образцу, 
// а также создания шаблона вершин
//--------------------------------------------------------------------------------------
HRESULT CreateVertexShader(WCHAR * szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, 
	ID3D11VertexShader** ppVertexShader, ID3D11InputLayout** ppVertexLayout)
{
	// Устройство (для создания объектов) берется из namespace Meshes
	ID3D11Device* Device = Meshes::g_pd3dDevice;

	ID3DBlob* pBlob = NULL; // Вспомогательный объект - просто место в оперативной памяти

	// Компиляция вертексного шейдера из файла
	HRESULT hr = CompileShaderFromFile(szFileName, szEntryPoint, szShaderModel, &pBlob);
    if (FAILED(hr))
    {
        MessageBox( NULL, L"Невозможно скомпилировать файл FX: Пожалуйста, запустите данную программу из папки, содержащей файл FX.",
			L"Ошибка", MB_OK );
        return hr;
    }

	// Создание вертексного шейдера 
	hr = Device->CreateVertexShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, ppVertexShader );
    if (FAILED(hr))
	{	
		pBlob->Release();
        return hr;
	}

    // Создание шаблона вершин	(layout и numElements определены в файле Meshes.h)
	hr = Device->CreateInputLayout( layout, numElements, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), ppVertexLayout );
	pBlob->Release();
	if (FAILED(hr)) return hr;

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Вспомогательная функция для компиляции и создания пиксельных шейдеров по единому образцу
//--------------------------------------------------------------------------------------
HRESULT CreatePixelShader(WCHAR * szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel,
	ID3D11PixelShader** ppPixelShader)
{
	// Устройство (для создания объектов) берется из namespace Meshes
	ID3D11Device* Device = Meshes::g_pd3dDevice;

	ID3DBlob* pBlob = NULL; // Вспомогательный объект - просто место в оперативной памяти

	// Компиляция пиксельного шейдера из файла
	HRESULT hr = CompileShaderFromFile(szFileName, szEntryPoint, szShaderModel, &pBlob);
    if (FAILED(hr))
    {
        MessageBox( NULL, L"Невозможно скомпилировать файл FX. Пожалуйста, запустите данную программу из папки, содержащей файл FX.",
			L"Ошибка", MB_OK );
        return hr;
    }

	// Создание пиксельного шейдера 
	hr = Device->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, ppPixelShader );
	if (FAILED(hr)) return hr;
	pBlob->Release();

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Вспомогательная функция УСТАНОВКИ буферов и пиксельного шейдера для любого заданного объекта 
//--------------------------------------------------------------------------------------
void SetBuffers(ID3D11Buffer* VertexBuffer, ID3D11Buffer* IndexBuffer, ID3D11PixelShader* PixelShader)
{
		// Контекст (устройство рисования), адрес и регистр обновляемого константного буфера берутся из namespace Meshes
	ID3D11DeviceContext* Context = Meshes::g_pImmediateContext;
	ID3D11Buffer* pConstantBuffer = Meshes::g_pConstantBuffer;
	void* pcb = Meshes::P_cb;
	int BufferIndex = Meshes::ConstantBufferRegisterIndex;

		// Установка пиксельного шейдера
	Context->PSSetShader( PixelShader, NULL, 0 );

		// Установка константного буфера
	Context->UpdateSubresource( pConstantBuffer, 0, NULL, pcb, 0, 0 ); // загружаем cb в g_pConstantBuffer
	Context->VSSetConstantBuffers( BufferIndex, 1, &pConstantBuffer );	// задаем константный буфер для вершинного шейдера
	Context->PSSetConstantBuffers( BufferIndex, 1, &pConstantBuffer );	// задаем константный буфер для пиксельного шейдера

		// Установка буфера вершин
 	UINT stride = sizeof( VERTEX );
    UINT offset = 0;
	Context->IASetVertexBuffers( 0, 1, &VertexBuffer, &stride, &offset );

		// Установка буфера индексов
    Context->IASetIndexBuffer( IndexBuffer, DXGI_FORMAT_R16_UINT, 0 );

		// Установка способа отрисовки вершин в буфере
    Context->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
}



