#include"../ShaderHeader/BasicShaderHeader.hlsli"
#include"../ShaderHeader/FogShaderHeader.hlsli"
#include"../ShaderHeader/FogBufferHeader.hlsli"

float4 PSmain(ColorOutPut input) : SV_TARGET
{
    float4 fog = CaluFog(input.svpos, color, fogData.xyz, fogData.w);
    fog.a = 1;
    return fog;
}