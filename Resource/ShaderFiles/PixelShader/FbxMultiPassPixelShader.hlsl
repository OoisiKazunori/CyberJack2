#include"../ShaderHeader/FbxShaderHeader.hlsli"
#include"../ShaderHeader/MultiPassHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

TwoRender PSmain(VSOutput input)
{
    float4 texColor = float4(tex.Sample(smp, input.uv));
    
    TwoRender outPut;
    outPut.target0 = texColor;
    outPut.target0.a = color.a;
    outPut.target1 = float4(1.0f, 0.0f, 0.0f, 1.0f);
    
    return outPut;
}