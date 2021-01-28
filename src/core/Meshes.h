//--------------------------------------------------------------------------------------
// Заголовочный файл, содержащий описания структур, классов и функций,
// пригодных для работы с DirectX
// Для его использования нужно подключить к проекту также файл Meshes.cpp
//
// ДО вызова функций и методов модуля (включая конструкторы с параметрами)
// нужно присвоить глобальным переменным, входящим в namespace Meshes,
// значения активных устройств DirectX 
// Например:
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
// Глобальные переменные модуля (namespace Meshes)
// Им должны быть присвоены нужные ненулевые значения ДО вызова большинства функций и 
// методов модуля (включая конструкторы с параметрами)
//
// При использовании нескольких устройств или буферов, можно менять значения этих 
// глобальных переменных, задавая соответствующие значения перед вызовом функций.
// Однако конкретный объект, связанный при создании или заполнении с конкретным устройством, 
// не может быть прорисован на другом устройстве
//--------------------------------------------------------------------------------------
namespace Meshes
{
	extern ID3D11Device* g_pd3dDevice;					// Устройство (для создания объектов)
	extern ID3D11DeviceContext* g_pImmediateContext;	// Контекст (устройство рисования)

	// Следующие переменные используются только в функциях, меняющих матрицу мира
	// (семейство методов Draw, за исключением Draw_0; а также функция SetBuffers() )
	extern ID3D11Buffer* g_pConstantBuffer;				// Константный буфер, обновляемый вместе с матрицей мира
	extern void* P_cb;				// Указатель на структуру, используемую для заполнения этого буфера
	extern int Size_cb;				// Размер этой структуры в байтах 
	extern DirectX::XMMATRIX* g_pWorld;		// Указатель на матрицу мира, входящую в состав этой структуры
	extern int ConstantBufferRegisterIndex;	// Индекс регистра b (в GPU), куда передается этот константный буфер
}

//--------------------------------------------------------------------------------------
// Структура вершины. Допускается внесение в нее изменений, но это может 
// потребовать также правки исходного кода методов в файле Meshes.cpp
//--------------------------------------------------------------------------------------
struct VERTEX
{
	DirectX::XMFLOAT3 Pos;		// Координаты точки в пространстве
	DirectX::XMFLOAT3 TexPos;	// Координаты точки в текстуре
	DirectX::XMFLOAT3 Norm;		// Нормаль к поверхности в данной точке
	DirectX::XMFLOAT2 TexNormPos; //Координаты точки в "текстуре" нормалей (для некоторых пиксельных шейдеров)
};

    // Определение шаблона вершин					// (слово const необходимо во избежание ошибки линкера, т.к. 
const D3D11_INPUT_ELEMENT_DESC layout[] =			// этот заголовочный файл подключается одновременно в разных модулях) 
{													
	{ "POSITION",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXPOSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "NORMAL",		 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXNORMAL",	 0, DXGI_FORMAT_R32G32_FLOAT,	 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
const UINT numElements = ARRAYSIZE( layout );



//--------------------------------------------------------------------------------------
// Функции для компиляции шейдеров, создания и установки буферов
//--------------------------------------------------------------------------------------

	// Вспомогательная функция для компиляции шейдеров в D3DX11 (взята из туториала без изменений,
	//  не считая удаления неактивного препроцессорного блока; не использует Meshes.h)
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut );

	// Функция для компиляции и создания вертексных шейдеров, а также создания шаблона вершин
HRESULT CreateVertexShader(WCHAR * szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, 
	ID3D11VertexShader ** ppVertexShader, ID3D11InputLayout** ppVertexLayout);

	// Функция для компиляции и создания пиксельных шейдеров
HRESULT CreatePixelShader(WCHAR * szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, 
	ID3D11PixelShader ** ppPixelShader);

	// Вспомогательная функция для создания любых буферов
HRESULT CreateBuffer(void* source, int size, UINT BindFlags, ID3D11Buffer** destination);

	// Функция УСТАНОВКИ буферов (вершинного, индексного и константного) и пиксельного шейдера 
	// для любого заданного объекта (вызывается в методе Draw(), но может использоваться и отдельно)
void SetBuffers(ID3D11Buffer* VertexBuffer, ID3D11Buffer* IndexBuffer, ID3D11PixelShader* PixelShader);



//--------------------------------------------------------------------------------------
// Основной класс для фигур
//--------------------------------------------------------------------------------------
class Mesh
{
public:
		// Конструкторы и деструкторы
	Mesh();
	Mesh(const VERTEX *Vertices, int NVertices, const WORD *Indices, int NIndices, int SetNormal = 0, 
		ID3D11PixelShader *PixelShader = NULL);
	Mesh(const char * FileName, ID3D11PixelShader *PixelShader = NULL);
	~Mesh();

		// Освобождение памяти
	void Release();

		// Задание параметров
	void SetVerticesIndices(const VERTEX *Vertices, int NVertices, const WORD *Indices, int NIndices, int SetNormal = 0);
	void CalculateNormals();
	void CreateVerticesIndicesBuffers();
	void SetWorld(DirectX::CXMMATRIX World);
	void SetPixelShader(ID3D11PixelShader *PixelShader);

		// Извлечение параметров
	VERTEX * GetVertices();
	WORD * GetIndices();
	int GetNVertices();
	int GetNIndices();
	ID3D11Buffer * GetVertexBuffer();
	ID3D11Buffer * GetIndexBuffer();
	DirectX::XMMATRIX GetWorld();
	ID3D11PixelShader * GetPixelShader();

		// Загрузка меша из файла
	void LoadFromObj(const char * FileName);		// Загрузка меша из файла .obj

		// Прорисовка на экране
	void Draw_0();					// Матрица мира не изменяется (используется текущее значение);
									// не устанавливаются буфера и пиксельный шейдер (в отличие от других методов прорисовки)
	void Draw();					// Матрица мира = полю world
	void Draw(DirectX::CXMMATRIX World);		// Матрица мира = (поле world) * (параметр World) , чтобы учесть их оба

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
// Класс для конуса
//--------------------------------------------------------------------------------------
class Cone : public Mesh
{
public:
	Cone(double h, double r, int ny, int nFi);
};


//--------------------------------------------------------------------------------------
// Класс для прямоугольника с текстом
//--------------------------------------------------------------------------------------
class TextMesh : public Mesh
{
public:
	void Print(char * Text);

		// Текст на конусе: 
		// hRel1, hRel2 - относительная высота верхнего и нижнего края текста (в долях высоты конуса), 
		// fi0 - угловое положение левого края текста (отсчитывается от оси X против часовой стрелки)
		// Magnify - коэффициент увеличения радиуса текста по сравнению с конусом (чтобы он не поглотился конусом)
	void PrintCone(char * Text, Cone & C, double hRel1, double hRel2, double fi0, double Magnify);

		// Текст на воображаемой поверхности вращения:
		// h1, h2 - координаты верхнего и нижнего края текста по оси Y,
		// r1, r2 - радиусы окружности для верхнего и нижнего краев текста (вокруг оси Y)
		// fi0 - угловое положение левого края текста (отсчитывается от оси X против часовой стрелки)
	void PrintCone(char * Text, double h1, double r1, double h2, double r2, double fi0);	
};


//--------------------------------------------------------------------------------------
// Класс для объектов (инстанций)
//--------------------------------------------------------------------------------------
class Instance
{
public:
		// Указатель на меш, соответствующий данной инстанции
	Mesh * mesh;

		// Конструкторы и деструкторы
	Instance(); 
	Instance(Mesh * M); 
	Instance(Mesh * M, DirectX::CXMMATRIX World);
	~Instance();

		// Задание параметров
	void SetWorld(DirectX::CXMMATRIX World);

		// Извлечение параметров
	DirectX::XMMATRIX GetWorld();

		// Прорисовка на экране
	void Draw_0();		// Матрица мира не изменяется (используется текущее значение); 
						// не устанавливаются буфера и пиксельный шейдер
	void Draw();		// Матрица мира = (поле world меша) * (поле world инстанции) , чтобы учесть их оба; 
						// устанавливаются буфера и пиксельный шейдер
			// Для более сложных действий с матрицами можно действовать прямо через поле mesh

protected:
	DirectX::XMFLOAT4X4 world;
};

#endif
