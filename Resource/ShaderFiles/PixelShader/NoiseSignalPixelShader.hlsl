#include"../ShaderHeader/SpriteShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float Rand(float2 coord, float seed)
{
    return frac(sin(dot(coord.xy, float2(12.9898, 78.233)) + seed) * 43758.5453);
}

cbuffer Seed : register(b1)
{
    float seed;
}

float4 PSmain(VSOutput input) : SV_TARGET
{
    float4 output = float4(tex.Sample(smp, input.uv));

    //シード値が-1以外なら砂嵐、-1でテクスチャを出す
    if(seed != -1)
    {
        //砂嵐
        float2 randamNum = input.uv;
        randamNum *= sin(seed);
        //乱数
        float r = Rand(input.uv, seed); // = rand(randamNum);
    
        //砂嵐作成
        float3 noiseSignal = float3(r, r, r);
        float noiseIntensity = 1.0f;
        //mix
        output.rgb = lerp(float3(output.rgb), noiseSignal, noiseIntensity);

        output.a = color.a;
    }

    return output;
}