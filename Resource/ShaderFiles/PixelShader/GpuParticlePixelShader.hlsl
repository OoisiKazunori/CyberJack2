#include"../ShaderHeader/GPUParticleHeader.hlsli"

TwoRender PSmain(PosColorOutput input)
{
    TwoRender output;
    output.target0 = input.color;
    output.target1 = float4(1.0f,0.0f,0.0f,1.0f);
    return output;
}