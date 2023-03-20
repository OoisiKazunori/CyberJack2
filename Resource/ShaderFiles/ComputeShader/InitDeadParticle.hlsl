#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/ExcuteIndirectHeader.hlsli"
#include"../ShaderHeader/DeadParticleData.hlsli"

struct ParticleData
{
    float3 pos;
    float4 color;
    uint id;
};

//çXêV
RWStructuredBuffer<ParticleData> particleData : register(u0);
AppendStructuredBuffer<InitData> initParticleData : register(u1);

cbuffer RootConstants : register(b0)
{
    uint meshID;
    uint vertNum;
};

[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = groupThreadID.x;
    index += 1024 * groupId.x;

    if(vertNum <= index)
    {
        return;
    }

    InitData outputData;
    outputData.pos = particleData[index].pos;
    outputData.color = float4(1,1,1,1);
    outputData.vel = RandVec3(index,2,-2) * 0.1f;
    outputData.meshID = meshID;

    initParticleData.Append(outputData);
}