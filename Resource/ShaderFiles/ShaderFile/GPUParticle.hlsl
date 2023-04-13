#include"../ShaderHeader/GPUParticleHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

PosUvColorOutput VSmain(float4 pos : POSITION,float2 uv : TEXCOORD,uint id : SV_INSTANCEID)
{
    PosUvColorOutput op;
    op.svpos = mul(matrixData[id].mat, pos);
    op.uv = uv;
    op.color = matrixData[id].color;
    return op;
}

float4 PSmain(PosUvColorOutput input) : SV_TARGET
{
    float4 texColor = tex.Sample(smp,input.uv);
    //texColor.xyz = input.color.xyz;
    //texColor.xyz = float3(1,1,1);
    texColor.a = 1.0f;
    return texColor;
}