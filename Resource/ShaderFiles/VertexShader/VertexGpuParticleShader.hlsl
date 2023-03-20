#include"../ShaderHeader/GPUParticleHeader.hlsli"

ColorOutPut VSmain(float4 pos : POSITION)
{
    ColorOutPut op;
    op.svpos = mul(mat, pos);
    return op;
}