#include"../ShaderHeader/SpriteShaderHeader.hlsli"
#include"../ShaderHeader/FogShaderHeader.hlsli"
#include"../ShaderHeader/MultiPassHeader.hlsli"

Texture2D<float4> flameTex : register(t0);
Texture2D<float4> windowTex : register(t1);
SamplerState smp : register(s0);

static const float2 LEFT_UP_UV =    float2(0.015f,0.12f);
static const float2 LEFT_DOWN_UV =  float2(0.015f,0.99f);
static const float2 RIGHT_UP_UV =   float2(0.985f,0.12f);
static const float2 RIGHT_DOWN_UV = float2(0.985f,0.99f);

cbuffer Seed : register(b1)
{
    float seed;
    float4 fogData;
}

float Rand(float2 coord, float seed)
{
    return frac(sin(dot(coord.xy, float2(12.9898, 78.233)) + seed) * 43758.5453);
}

TwoRender PSmain(VSOutput input)
{
    float4 outputNoiseInWindow = float4(0.0f,0.0f,0.0f,0.0f);
    if(seed != -1)
    {
        //遐ょｵ?
        float2 randamNum = input.uv;
        randamNum *= sin(seed);
        //荵ｱ謨ｰ
        float r = Rand(input.uv, seed); // = rand(randamNum);
        //遐ょｵ蝉ｽ懈??
        float3 noiseSignal = float3(r, r, r);
        float noiseIntensity = 1.0f;
        //mix
        outputNoiseInWindow = float4(lerp(float3(1,1,1), noiseSignal, noiseIntensity),1.0f);
    }


    TwoRender output;
    output.target1 = float4(0,0,0,0);

    //枠内にテクスチャを描画
    if(LEFT_UP_UV.x <= input.uv.x && LEFT_UP_UV.y <= input.uv.y &&
        LEFT_DOWN_UV.x <= input.uv.x && input.uv.y <= LEFT_DOWN_UV.y &&
        input.uv.x <= RIGHT_UP_UV.x && RIGHT_UP_UV.y <= input.uv.y &&
        input.uv.x <= RIGHT_DOWN_UV.x && input.uv.y <= RIGHT_DOWN_UV.y)
    {
        float2 inUv = (input.uv - LEFT_UP_UV) / RIGHT_DOWN_UV;

        if(seed != -1)
        {
            output.target0 = CaluFog(input.svpos, outputNoiseInWindow, fogData.xyz, fogData.w);
            return output;
        }
        else
        {
            output.target0 = CaluFog(input.svpos, float4(windowTex.Sample(smp, inUv)), fogData.xyz, fogData.w);
            return output;
        }
    }

    output.target0 = CaluFog(input.svpos, float4(flameTex.Sample(smp, input.uv)), fogData.xyz, fogData.w);
    //ウインドウ部分の出力
    return output;
}