#include"../ShaderHeader/SpriteShaderHeader.hlsli"

Texture2D<float4> mainTex : register(t0);
Texture2D<float4> markTex : register(t1);
SamplerState smp : register(s0);

float4 PSmain(VSOutput input) : SV_TARGET
{
    float4 tex = float4(mainTex.Sample(smp, input.uv));
    float4 luminanceData = float4(markTex.Sample(smp, input.uv));
    
        
    float4 output = float4(0.0f, 0.0f, 0.0f, 1.0f);
    if (0.1f <= luminanceData.r)
    {
        output = tex;

        if(luminanceData.a <= output.a)
        {
            output.a = luminanceData.a;
        }
    }


    return output;
}