#include"../ShaderHeader/FbxShaderHeader.hlsli"
#include"../ShaderHeader/FogShaderHeader.hlsli"
#include"../ShaderHeader/bgHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 PSmain(VSOutput input) : SV_TARGET
{
    float3 light = normalize(float3(1, -1, 1));
    float diffuse = saturate(dot(-light, input.normal));
    float brightness = diffuse + 0.3f;
    float4 texColor = float4(tex.Sample(smp, input.uv));
    
    //フォグ
    float4 fog = CaluFog(input.svpos, texColor, fogData.xyz, fogData.w);
    //return float4(brightness, brightness, brightness, 1);
    return fog;
}