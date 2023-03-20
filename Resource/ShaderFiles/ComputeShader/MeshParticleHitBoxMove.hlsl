#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/GPUParticle.hlsli"
#include"../ShaderHeader/MeshCollision.hlsli"

//メッシュパーティクル
RWStructuredBuffer<ParticleHitBoxData> particleData : register(u0);
//親子関係
RWStructuredBuffer<matrix> motherMatData : register(u1);

[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = groupThreadID.x;
    index += 1024 * groupId.x;

    if(20000 < index)
    {
        return;
    }
    
    uint meshID = particleData[index].meshID;
    float3 pos = particleData[index].pos;
    particleData[index].pos = UpdatePos(motherMatData[meshID],particleData[index].pos);
}
