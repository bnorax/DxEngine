cbuffer ConstantBuffer : register(b1)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 colorLight;
	float3 directionLight;
	float time;
};	
//--------------------------------------------------------------------------------------
struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
	float2 Texcoord : TEXCOORD;
	float3 Normal : NORMAL;

};

struct VS_INPUT
{
	float4 Pos : POSITION;
	float4 Color : COLOR;
	float2 Texcoord : TEXCOORD;
	float3 Normal : NORMAL;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT vs_main(VS_INPUT input)
{
	PS_INPUT output;
	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.Color = input.Color;
	output.Texcoord = input.Texcoord;
	output.Normal = normalize(input.Normal);
	return output;
}


float wavef(float2 p, float angle)
{
    float2 direction = float2(sin(angle), cos(angle));
    return cos(dot(p, direction));
}

float wrapf(float x)
{
    return abs(fmod(x, 2.0f) - 1.0f);
}

PS_INPUT vs_wave(VS_INPUT input)
{
	PS_INPUT output;

    float A = 2.0f, L = 10.0f, S = 0.1f;
    float2 D = { 0.6f, 0.0f };



	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);

	output.Color = input.Color;
	output.Texcoord = input.Texcoord;
	output.Normal = normalize(input.Normal);
	return output;
}