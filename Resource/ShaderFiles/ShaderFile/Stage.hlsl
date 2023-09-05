#include"ModelBuffer.hlsli"

struct PosUvNormalTangentBinormalOutput
{
    float4 svpos : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 worldPos : POSITION;
    float3 tangent : TANGENT2;
    float3 binormal : BINORMAL;
};

struct VertexData
{
    float4 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    int4 boneNo : BONE_NO;
    float4 weight : WEIGHT;
};

cbuffer MatBuffer : register(b0)
{
    matrix worldMat;
    matrix viewMat;
    matrix projectionMat;
    matrix rotaion;
}

cbuffer ColorBuffer : register(b1)
{
    float4 color;
}

cbuffer WorldPosBuffer : register(b2)
{
    float3 leftUpPos;
    float3 rightDownPos;
}

Texture2D<float4> lightTex:register(t4);

PosUvNormalTangentBinormalOutput VSModel(VertexData input)
{
    float4 resultPos = input.pos;
    PosUvNormalTangentBinormalOutput op;
    op.svpos = mul(worldMat,resultPos);
    op.worldPos = op.svpos.xyz;
    op.svpos = mul(viewMat,op.svpos);
    op.svpos = mul(projectionMat,op.svpos);
    op.uv = input.uv;
    op.normal = input.normal;
    op.binormal = input.binormal;
    op.tangent = input.tangent;
    return op;
}

GBufferOutput PSModel(PosUvNormalTangentBinormalOutput input) : SV_TARGET
{
    float4 texColor = AlbedoTex.Sample(smp,input.uv);

    GBufferOutput output;
    output.albedo = texColor * color;
    output.normal = float4(input.normal, 1.0f);
    output.metalnessRoughness = float4(0,0,0,1.0f);
    output.world = float4(input.worldPos,1.0f);
    output.emissive = float4(0.0f,0.0f,0.0f,1.0f);

    //白色をマスクに色を変える
    if(1.0f <= texColor.x &&
        1.0f <= texColor.y &&
        1.0f <= texColor.z)
    {
        float3 maxRate = rightDownPos - leftUpPos;
        float3 nowRate = input.worldPos - leftUpPos;
        float3 rate = nowRate / maxRate;
        rate.x = clamp(rate.x,0.0f,1.0f);
        rate.z = clamp(rate.z,0.0f,1.0f);
        float4 emissve = lightTex.Sample(smp,float2(rate.x,rate.z));
        output.albedo = emissve;
        output.albedo.w = 1.0f;
        output.emissive = emissve;
    }
    return output;
}