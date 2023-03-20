#include"../ShaderHeader/OBJShaderHeader.hlsli"
#include"../ShaderHeader/BaseShaderHeader.hlsli"
#include"../ShaderHeader/FogShaderHeader.hlsli"

SamplerState smp : register(s0);
Texture2D<float4>tex:register(t0);

cbuffer fogBuffer : register(b2)
{
    //フォグカラー
    float3 endColor;
    float3 subValue;
    //x...RataMin,y...RateMax,z...StartFogDepth,w...EndFogDepth
    float4 rateAndFogLine;
    float3 depthX;
}

float4 PSmain(FogObjOutput input) : SV_TARGET
{
    float4 texColor = tex.Sample(smp, input.uv);

    float3 fog = CalucurateFogValue(input.fogFactor, endColor, subValue, texColor.xyz, rateAndFogLine.x, rateAndFogLine.y, rateAndFogLine.z, rateAndFogLine.w, depthX.x, depthX.y, depthX.z);
    return float4(fog.xyz, color.a);
}