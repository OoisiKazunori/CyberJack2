#include"../ShaderHeader/FbxShaderHeader.hlsli"
#include"../ShaderHeader/MultiPassHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

cbuffer LightDirBuffer : register(b2)
{
    float3 lightDir;
}

TwoRender PSmain(VSOutput input)
{
    float3 light = normalize(lightDir);
    float diffuse = saturate(dot(-light, input.normal));
    float brightness = diffuse + 0.5f;

    float4 texColor = float4(tex.Sample(smp, input.uv));
    
    TwoRender outPut;
    outPut.target0 = texColor * brightness;
    outPut.target0.a = color.a;
    outPut.target1 = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    return outPut;
}