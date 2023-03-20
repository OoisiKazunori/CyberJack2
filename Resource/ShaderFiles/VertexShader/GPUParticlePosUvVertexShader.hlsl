#include"../ShaderHeader/GPUParticleHeader.hlsli"

PosUvOutput VSmain(float4 pos : POSITION,float2 uv : TEXCOORD,uint id : SV_INSTANCEID)
{
    PosUvOutput op;
    op.svpos = mul(matrixData[id].mat, pos);
    op.uv = uv;
    return op;
}