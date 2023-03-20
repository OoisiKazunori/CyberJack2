#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/ExcuteIndirectHeader.hlsli"
#include"../ShaderHeader/GPUParticle.hlsli"

//“ü—Í
RWStructuredBuffer<float3>vertData : register(u0);
//o—Í
RWStructuredBuffer<ParticleData> updateParticleData : register(u1);

[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = ThreadGroupIndex(groupId,groupIndex,groupThreadID,1024);
    ParticleData inputData;
    inputData.pos = vertData[index];
    inputData.color = float4(1,1,1,1);
    inputData.id = 0;
    updateParticleData[index] = inputData;
}
