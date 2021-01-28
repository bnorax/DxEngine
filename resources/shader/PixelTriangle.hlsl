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
	//float4 colorLight = { 1.0f, 1.0f, 1.0f, 1.0f };
	//float3 directionLight = { 0.0f, 0 , -1.0f };
	float4 textureColor = Tex.Sample(Samp, input.Texcoord);
	float intensity = saturate(dot(input.Normal, -directionLight));
	float4 light = saturate(colorLight*intensity);
    float4 finalColor = light * textureColor;
    finalColor.a = textureColor.a;
    return finalColor;
    //light * textureColor

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

float4 ps_wave_dist(PS_INPUT input) : SV_Target
{
    float4 finalColor;
    float4 textureColor = Tex.Sample(Samp, input.Texcoord);
    float2 pos = input.Pos.xy/4.;
    float br = 0.;
    
    for (float i = 1.; i <= 4.; i++)
    {
        br += wavef(pos, time / i/10000);
    }
    br = wrapf(br);
    float a = 0.0;
    finalColor.rgb = (1-a)*br + a*textureColor.rgb;
    finalColor.a = 1;
    return finalColor;
}

float4 ps_main_notexture(PS_INPUT input) : SV_Target
{
	float intensity = saturate(dot(input.Normal, -directionLight));
	float4 light = saturate(colorLight*intensity);
	return light;
}