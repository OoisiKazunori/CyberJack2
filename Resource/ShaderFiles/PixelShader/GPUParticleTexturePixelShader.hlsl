#include"../ShaderHeader/GPUParticleHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

TwoRender PSmain(PosUvOutput input)
{
    TwoRender output;
    output.target0 = float4(tex.Sample(smp, input.uv));
    output.target1 = float4(1.0f,0.0f,0.0f,1.0f);
    return output;
}