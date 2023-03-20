#include"../ShaderHeader/BasicShaderHeader.hlsli"
#include"../ShaderHeader/InstanceHeader.hlsli"
#include"../ShaderHeader/MultiPassHeader.hlsli"

cbuffer BloomData : register(b1)
{
    float4 luminanceColor;
}

TwoRender PSmain(InstancePosOut input)
{
    TwoRender output;
    output.target0 = uavMatColorData[input.id].color;
    output.target1 = luminanceColor;
    return output;
}