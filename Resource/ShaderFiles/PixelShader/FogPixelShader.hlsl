#include"../ShaderHeader/BaseShaderHeader.hlsli"
#include"../ShaderHeader/FogShaderHeader.hlsli"
#include"../ShaderHeader/FogBufferHeader.hlsli"

float4 PSmain(FogOutPut input) : SV_TARGET
{
    float4 fog = CaluFog(input.svpos, color, fogData.xyz, fogData.w);
    return fog;
}