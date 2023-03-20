#include"../ShaderHeader/BasicShaderHeader.hlsli"
#include"../ShaderHeader/MultiPassHeader.hlsli"

cbuffer BloomData : register(b1)
{
    float4 luminanceColor;
}

TwoRender PSmain(ColorOutPut input)
{
    TwoRender output;
    output.target0 = color;
    output.target1 = float4(luminanceColor.xyz, 1.0f);
    return output;
}