#include"../ShaderHeader/SpriteShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 PSmain(VSOutput input) : SV_TARGET
{
    float4 output = float4(tex.Sample(smp, input.uv));
    if (color.a < output.a)
    {
        output.a = color.a;
    }
    return output;
}