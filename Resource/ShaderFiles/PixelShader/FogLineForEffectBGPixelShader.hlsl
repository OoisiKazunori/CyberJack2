#include"../ShaderHeader/BasicShaderHeader.hlsli"
#include"../ShaderHeader/FogShaderHeader.hlsli"
#include"../ShaderHeader/FogBufferHeader.hlsli"

float4 PSmain(ColorOutPut input) : SV_TARGET
{
    float3 fogC = float3(229.0f / 255, 141.0f / 255, 24.0f / 255);
    //float3 fogC = float3(255.0f, 0.0f, 0.0f);
    
    float4 fog = CaluFog(input.svpos, color, fogC, 0.0005);
    fog.a = 1;
    return fog;
}