#include"../ShaderHeader/SpriteShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);
float4 PSmain(VSOutput input) : SV_TARGET
{
    float4 sprite = float4(tex.Sample(smp, input.uv));
    
     
    //uv座標のy軸の0と1に近づけば近づくほど色が薄くなる
    if (input.uv.y < 5.0)
    {
        sprite.a = input.uv.y;
    }
    else
    {
        sprite.a = (1.0 - input.uv.y);
    }

    if (sprite.a < color.a)
    {
        //return sprite;
    }
    else
    {
        sprite.a = color.a;
    }   
    
    
    //白線の移動
    //exp.uvが2.0~3.0の間なら白色にする(2.0と3.0は定数バッファで決める)
    float uv = 0, uv2 = 0;
    if (uv < input.uv.y && input.uv.y < uv2)
    {
        sprite.r = 1.0;
        sprite.g = 1.0;
        sprite.b = 1.0;
    }
    
    
    return sprite;
}