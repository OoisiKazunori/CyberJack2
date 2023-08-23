#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/GPUParticle.hlsli"


struct VertexBufferData
{
    float3 pos;
    float3 normal;
    float2 uv;
    float3 tangent;
    float3 binormal;
    int4 boneNo;
    float4 weight;
};

struct VertexData
{
	float3 pos;
    uint id;
};

//����
RWStructuredBuffer<VertexBufferData> VertexBuffer : register(u0);
AppendStructuredBuffer<VertexData> OutputBuffer : register(u1);

cbuffer cbuff0 :register(b0)
{
    uint id;
    uint vertMaxNum;
}

[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = ThreadGroupIndex(groupId,groupIndex,groupThreadID,1024);

    if(vertMaxNum < index)
    {
        return;
    }

    VertexData output;
    output.id = id;
    output.pos = VertexBuffer[index].pos;
    OutputBuffer.Append(output);
}
