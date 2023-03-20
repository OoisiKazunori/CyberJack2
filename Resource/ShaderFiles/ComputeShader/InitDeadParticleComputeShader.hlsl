#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/ExcuteIndirectHeader.hlsli"


struct InitData
{
    float4 pos;
    float4 color;
    float4 vel;
};

//çXêV
RWStructuredBuffer<float3> particleData : register(u0);
RWStructuredBuffer<InitData> initParticleData : register(u1);

[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = groupThreadID.x;
    index += 1024 * groupId.x;

    initParticleData[index].pos.xyz = particleData[index];
    initParticleData[index].color = float4(1,1,1,1);
    initParticleData[index].vel.xyz = RandVec3(index,2,-2) * 0.1f;
}