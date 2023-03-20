#include"../ShaderHeader/SpriteShaderHeader.hlsli"
#include"../ShaderHeader/MultiPassHeader.hlsli"

cbuffer lineData : register(b1)
{
    float4 distance;
    float4 hitColor;
    uint firstFlag;
}

TwoRender PSmain(VSOutput input)
{
    float4 outputColor;
    if (input.uv.y < distance.x)
    {
        outputColor = hitColor;
    }
    else
    {
        outputColor = color;
    }
    if(firstFlag)
    {
        outputColor.a = input.uv.y;
    }

    TwoRender output;
    output.target0 = outputColor;
    output.target1 = float4(0.0f, 0.0f, 0.0f, 1.0f);
    return output;
}