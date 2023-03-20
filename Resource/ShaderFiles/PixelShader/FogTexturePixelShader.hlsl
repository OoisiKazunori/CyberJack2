#include"../ShaderHeader/SpriteShaderHeader.hlsli"
#include"../ShaderHeader/FogShaderHeader.hlsli"
#include"../ShaderHeader/FogBufferHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 PSmain(VSOutput input) : SV_TARGET
{
    float4 test = float4(tex.Sample(smp, input.uv));
    
    float3 fogC = float3(0.0862745121, 0.149019614, 0.145098045);
    
    float4 fog = CaluFog(input.svpos, test, fogC, 0.002);
    fog.a = color.a;
    return fog;
}