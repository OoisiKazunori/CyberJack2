#include"../ShaderHeader/BasicShaderHeader.hlsli"
#include"../ShaderHeader/FogShaderHeader.hlsli"
#include"../ShaderHeader/FogBufferHeader.hlsli"

float4 PSmain(ColorOutPut input) : SV_TARGET
{
    float3 fogdata = float3(fogData.x, fogData.y, fogData.z);
    float3 fogC = float3(0.0862745121, 0.149019614, 0.145098045);
    
    float4 tmpColor = float4(color.x, color.y, color.z, 1);
    float4 fog = CaluFog(input.svpos, tmpColor, fogC, 0.002);
    fog.a = color.a;
    return fog;
}