#include"../ShaderHeader/SpriteShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 PSmain(ClipTexOutput input) : SV_TARGET
{
    float2 uv = input.screenPos.xy / input.screenPos.w;
    float4 output = float4(tex.Sample(smp, uv));
    return output;
}