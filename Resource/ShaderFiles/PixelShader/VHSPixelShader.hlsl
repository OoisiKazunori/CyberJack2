#include"../ShaderHeader/SpriteShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);
float WhiteNoise(float2 coord)
{
    return frac(sin(dot(coord, float2(8.7819, 3.255))) * 437.645);
}

cbuffer time : register(b1)
{
    float Time;
}

float4 PSmain(VSOutput input) : SV_TARGET
{
    //樽状湾曲-----------------------------------------------------------
    //歪み率
    float Distortion = 0.06f;
    float VertGlitchPase = 1.0f;
    float HorzGlitchPase = 1.0f;
    float GlitchStepValue = 1.0f;
    
    float vertNoise = WhiteNoise(float2(floor((input.uv.x) / VertGlitchPase) * VertGlitchPase, Time * 0.6));

    //樽状湾曲をかけた上でサンプリング
    float2 samplePoint = input.uv;
    samplePoint -= float2(0.5, 0.5);
    float distPower = pow(length(samplePoint), Distortion);
    samplePoint *= float2(distPower, distPower);
    samplePoint += float2(0.5, 0.5);
    float4 Tex = tex.Sample(smp, samplePoint);
    
    //エフェクトがかからない部分を作るための計算。上に走っていく横線とする
    float sinv = sin(samplePoint.y * 2 - Time * -0.1);
    float steped = 1 - step(0.99, sinv * sinv);
    
    //走査線ノイズ
    Tex.rgb -= abs(sin(input.uv.y * 100.0 + Time * 5.0)) * 0.08;
    Tex.rgb -= abs(sin(input.uv.y * 300.0 - Time * 10.0)) * 0.05;

    //全体にビネットをかける
    float vignette = length(float2(0.5, 0.5) - input.uv);
    vignette = clamp(vignette - 0.46f, 0, 1);
    Tex.rgb -= vignette;

    //青緑っぽい画面にする
    Tex.b += 0.01f;
    Tex.g += 0.06f;
    
    //Tex.a = color.a;
    //出力
    return Tex;

}