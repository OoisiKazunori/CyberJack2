#include"ModelBuffer.hlsli"

struct OutputData
{
    matrix mat;
    float4 color;
};
struct VertexBufferData
{
    float3 svpos;
    float3 normal;
    float2 uv;
    float3 tangent;
    float3 binormal;
};

RWStructuredBuffer<OutputData> matrixData : register(u0);

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float3 normal : NORMAL;
    float3 worldpos : WORLDPOS;
    float4 color : COLOR;
};

struct VertexInput
{
    float3 svpos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

VSOutput VSmain(VertexInput input, uint id : SV_INSTANCEID)
{
    VSOutput op;
    op.worldpos = input.svpos;
    op.svpos = mul(matrixData[id].mat, float4(input.svpos, 1.0f));
    op.color = matrixData[id].color;
    op.normal = input.normal;
    return op;
}

GBufferOutput PSmain(VSOutput input) : SV_TARGET
{
    GBufferOutput output;
    output.albedo = input.color;
    output.normal = float4(-1, -1, -1, 1.0f); //���������[���h���W�����Ȃ����߁A�����Ŗ@����-1,-1,-1�ɂ��邱�Ƃɂ����GPU�p�[�e�B�N���ł���Ƃ������Ƃ����C�g�����ɓ`���A�A���x�h�œh��Ԃ������������Ă��܂��B
    output.metalnessRoughness = float4(0, 1, 0, 1);
    output.world = float4(input.svpos.xyz, 1.0f);
    output.emissive = output.albedo / 2.0f;
    return output;
}


cbuffer buffer0 : register(b0)
{
    float4 emissive;
}
GBufferOutput PSEmissiveMain(VSOutput input) : SV_TARGET
{
    GBufferOutput output;
    output.albedo = input.color;
    output.normal = float4(-1, -1, -1, 1.0f); //���������[���h���W�����Ȃ����߁A�����Ŗ@����-1,-1,-1�ɂ��邱�Ƃɂ����GPU�p�[�e�B�N���ł���Ƃ������Ƃ����C�g�����ɓ`���A�A���x�h�œh��Ԃ������������Ă��܂��B
    output.metalnessRoughness = float4(0, 1, 0, 1);
    output.world = float4(input.svpos.xyz, 1.0f);
    output.emissive = float4(0.0f,1.0f,0.0f,1.0f);
    return output;
}

float4 PS(VSOutput input) : SV_TARGET
{
    return input.color;
}