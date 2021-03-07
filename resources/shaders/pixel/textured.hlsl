cbuffer ConstantBuffer : register(b1)
{
    matrix World;
    matrix View;
    matrix Projection;
    float4 colorLight;
    float3 directionLight;
    float time;
};


Texture2D Tex : register(t1);
SamplerState Samp : register(s1);

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
    float2 Texcoord : TEXCOORD;
    float3 Normal : NORMAL;
};

float4 ps_main(PS_INPUT input) : SV_Target
{
    float4 textureColor = Tex.Sample(Samp, input.Texcoord);
    float4 finalColor = textureColor;
    finalColor.a = textureColor.a;
    return finalColor;
}