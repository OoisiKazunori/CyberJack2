struct VSOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float2 uv : TEXCOORD; //uv値
};

cbuffer MatBuffer : register(b0)
{
    matrix Mat; //3D変換行列
}

cbuffer LightDir : register(b1)
{
    float3 WorldLightDir;
}

VSOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	VSOutput op;
	op.svpos = mul(Mat, pos);
	op.uv = uv;
	return op;
}

Texture2D<float4> AlbedoTex : register(t0);
Texture2D<float4> NormalTex : register(t1);
SamplerState smp : register(s0);

float4 PSmain(VSOutput input) : SV_TARGET
{
    float4 albedoColor = AlbedoTex.Sample(smp, input.uv);
    float4 worldNormalVec = NormalTex.Sample(smp, input.uv);

    float3 bright = dot(normalize(WorldLightDir),worldNormalVec.xyz);
    return float4(albedoColor.xyz * bright,albedoColor.a);
}