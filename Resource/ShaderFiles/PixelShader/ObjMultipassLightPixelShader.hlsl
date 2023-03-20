#include"../ShaderHeader/OBJShaderHeader.hlsli"
#include"../ShaderHeader/BaseShaderHeader.hlsli"
#include"../ShaderHeader/MultiPassHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

cbuffer cbuff2 : register(b2)
{
    float3 lightDir;
};

TwoRender PSmain(VSOutput input)
{
    float3 light = normalize(lightDir);
    float diffuse = saturate(dot(-light, input.normal));
    float brightness = diffuse + 0.9f;

    float4 texColor = tex.Sample(smp, input.uv);

    TwoRender output;
    output.target0 = texColor * brightness;
    output.target0.a = color.a;
    output.target1 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    return output;
}