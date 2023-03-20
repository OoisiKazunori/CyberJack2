#include"../ShaderHeader/SpriteShaderHeader.hlsli"
#include"../ShaderHeader/MultiPassHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

cbuffer uvBuffer : register(b1)
{
    float4 uv; //色
}


TwoRender PSmain(VSOutput input)
{
    float2 uvValue = input.uv + uv.xy;
    float4 t = float4(tex.Sample(smp, uvValue));
    
    //テクスチャの下の部分を見えなくする
    t.a = input.uv.y;
    if (color.a < t.a)
    {
        t.a = color.a;
    }
    
    TwoRender output;
    output.target0 = t;
    output.target1 = float4(1.0f, 0.0f, 0.0f, 1.0f);
    return output;
}