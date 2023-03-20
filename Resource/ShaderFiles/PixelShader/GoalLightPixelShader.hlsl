#include"../ShaderHeader/SpriteShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

cbuffer buffer1 : register(b1)
{
    float4 uvValue;
};

float4 PSmain(VSOutput input) : SV_TARGET
{
    //ここで定数バッファで値を転送しuvの値をずらしていく
    float2 tmpUv = input.uv;
    tmpUv.y += uvValue.x;    
    float4 sprite = float4(tex.Sample(smp, tmpUv));
    
    //uv座標のy軸に近づけば近づくほど透明になっていく
    sprite.a = (1.0 - input.uv.y);
    if (sprite.a < color.a)
    {
    }
    else
    {
        sprite.a = color.a;
    }
    
    return sprite;
}