#include"../ShaderHeader/SpriteShaderHeader.hlsli"

SamplerState smp : register(s0);

cbuffer GradationBuff : register(b1)
{
    float4 firstColor;
    float4 endColor;
}

float4 PSmain(VSOutput input) : SV_TARGET
{
    float4 sub = firstColor - endColor;
    sub *= float4(input.uv.y,input.uv.y, input.uv.y, 1.0f);
    return float4(endColor.x + sub.x, endColor.y + sub.y, endColor.z + sub.z, 1.0f);
}