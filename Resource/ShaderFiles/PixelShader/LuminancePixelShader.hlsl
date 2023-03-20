#include"../ShaderHeader/SpriteShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 PSmain(VSOutput input) : SV_TARGET
{
    float4 luminance = float4(tex.Sample(smp, input.uv));

    float4 checkValue = luminance - float4(1.0f, 1.0f, 1.0f, 0.0f);
    
    if (checkValue.x <= 0)
    {
        checkValue.x = 0.0f;
    }
    if (checkValue.y <= 0)
    {
        checkValue.y = 0.0f;
    }
    if (checkValue.z <= 0)
    {
        checkValue.z = 0.0f;
    }
    
    
    
    //‹P“x‚ª1.0ˆÈã‚È‚ç•`‰æ‚·‚é
    if ( 0.0f < checkValue.x ||
         0.0f < checkValue.y ||
         0.0f < checkValue.z)
    {
            return checkValue;
    }

    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}