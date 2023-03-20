#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/ExcuteIndirectHeader.hlsli"
#include"../ShaderHeader/GPUParticle.hlsli"
#include"../ShaderHeader/DeadParticleData.hlsli"

cbuffer RootConstants : register(b0)
{    
    matrix scaleRotateBillboardMat;
};

struct MotherData
{
    matrix motherMat;
    uint startFlag;
};

RWStructuredBuffer<InitData> particleData : register(u0);
RWStructuredBuffer<MotherData> motherMatData : register(u1);
//èoóÕ
AppendStructuredBuffer<GPUParticleInput> inputGPUParticleData : register(u2);

[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = groupThreadID.x;
    index += 1024 * groupId.x;

    particleData[index].pos += particleData[index].vel;
    particleData[index].color.a  += -0.01f;
    if(particleData[index].color.a <= 0.0f)
    {
        particleData[index].color.a = 0.0f;
        motherMatData[index].startFlag = true;
    }

    uint id = particleData[index].meshID;
    if(!motherMatData[id].startFlag)
    {
        return;
    }
    
    //Ë°åÂ?óË®àÁÆ?-------------------------
    matrix pMatWorld = scaleRotateBillboardMat;
    pMatWorld[0][3] = particleData[index].pos.x;
    pMatWorld[1][3] = particleData[index].pos.y;
    pMatWorld[2][3] = particleData[index].pos.z;
    

    pMatWorld = mul(CalucurateWorldMat(float3(0,0,50),float3(1,1,1),float3(0,0,0)),pMatWorld);

    //Ë°åÂ?óË®àÁÆ?-------------------------    
    GPUParticleInput outputMat;
    outputMat.worldMat = pMatWorld;
    outputMat.color = particleData[index].color;
    inputGPUParticleData.Append(outputMat);
}