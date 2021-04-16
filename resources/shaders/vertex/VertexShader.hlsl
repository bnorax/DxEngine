cbuffer cbPerRender : register(b0)
{
	matrix View;
	matrix Projection;
	float4 colorLight;
	float3 directionLight;
	float time;
};
cbuffer cbPerObject : register(b1)
{
	matrix World;
};
cbuffer cbBones : register(b2)
{
    matrix bones[500];
}
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
    uint4 boneIds : BONEID;
    float4 boneWiegth : BONEWEIGHT;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT vs_main(VS_INPUT input)
{
	PS_INPUT output;
   // matrix BoneTransform = bones[input.boneIds[0]] * input.boneWiegth[0];
    //BoneTransform += bones[input.boneIds[1]] * input.boneWiegth[1];
   //// BoneTransform += bones[input.boneIds[2]] * input.boneWiegth[2];
    //BoneTransform += bones[input.boneIds[3]] * input.boneWiegth[3];
   // output.Pos = mul(input.Pos, BoneTransform);
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
   //output.Pos = mul(BoneTransform, vp);
   
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