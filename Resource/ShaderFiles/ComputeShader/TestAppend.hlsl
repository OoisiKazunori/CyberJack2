#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/ExcuteIndirectHeader.hlsli"
#include"../ShaderHeader/GPUParticle.hlsli"

struct Float3Data
{
    float3 value;
};
//ì¸óÕ
AppendStructuredBuffer<Float3Data>particleInitData : register(u0);
//èoóÕ
RWStructuredBuffer<Float3Data> updateParticleData : register(u1);

[numthreads(5, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = ThreadGroupIndex(groupId,groupIndex,groupThreadID,1024);
    particleInitData.Append(updateParticleData[index]);
}
