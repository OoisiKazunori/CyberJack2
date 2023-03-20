#include"../ShaderHeader/BasicShaderHeader.hlsli"
#include"../ShaderHeader/InstanceHeader.hlsli"
#include"../ShaderHeader/FogShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

cbuffer FogData : register(b0)
{
    float4 fogData;
}

struct TwoRender
{
    float4 target0 : SV_TARGET0;
    float4 target1 : SV_TARGET1;
};

TwoRender PSmain(InstanceDepthOutPut input)
{
    float4 texColor = tex.Sample(smp, input.uv);

    TwoRender op;
    op.target0 = CaluFog(input.svpos, texColor, fogData.xyz, fogData.w); 
    op.target1 = float4(0,0,0,0);
    return op;
}