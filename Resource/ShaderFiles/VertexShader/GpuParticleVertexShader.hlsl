#include"../ShaderHeader/GPUParticleHeader.hlsli"

PosColorOutput VSmain(float4 pos : POSITION,float2 uv : TEXCOORD,uint id : SV_INSTANCEID)
{
    PosColorOutput op;
    op.svpos = mul(matrixData[id].mat, pos);
    op.color = matrixData[id].color;
    return op;
}