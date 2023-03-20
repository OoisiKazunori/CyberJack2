#include"../ShaderHeader/SpriteShaderHeader.hlsli"

cbuffer lineData : register(b1)
{
    float4 distance;
    float4 hitColor;
}

float4 PSmain(VSOutput input) : SV_TARGET
{
    if (input.uv.y < distance.x)
    {
        return hitColor;
    }
    return color;
}