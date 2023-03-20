#include"../ShaderHeader/BaseShaderHeader.hlsli"
#include"../ShaderHeader/FogShaderHeader.hlsli"
#include"../ShaderHeader/FogBufferHeader.hlsli"
#include"../ShaderHeader/MultiPassHeader.hlsli"

cbuffer BloomData : register(b2)
{
    float4 luminanceColor;
}

TwoRender PSmain(FogOutPut input)
{
    TwoRender op;
    op.target0 = CaluFog(input.svpos, color, fogData.xyz, fogData.w);
    op.target0.a = color.a;
    op.target1 = luminanceColor;
    return op;
}