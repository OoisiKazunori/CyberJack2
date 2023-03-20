#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/ExcuteIndirectHeader.hlsli"
#include"../ShaderHeader/GPUParticle.hlsli"

struct InitData
{
    float4 pos;
    float4 color;
    float4 vel;
};

struct MatData
{
    matrix mat;
    float4 color;
};

cbuffer RootConstants : register(b0)
{    
    matrix scaleRotateBillboardMat;
    matrix viewProjection;
    matrix motherMat;
};

//Êõ¥Êñ∞
RWStructuredBuffer<InitData> particleData : register(u0);
//èoóÕ
AppendStructuredBuffer<GPUParticleInput> inputGPUParticleData : register(u1);

[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = groupThreadID.x;
    index += 1024 * groupId.x;

    particleData[index].pos += particleData[index].vel;
    particleData[index].color.a  += -0.01f;

    if(particleData[index].color.a <= 0.0f)
    {
       particleData[index].pos.x = 100000.0f;
    }

    
    //Ë°åÂ?óË®àÁÆ?-------------------------
    matrix pMatWorld = scaleRotateBillboardMat;
    pMatWorld[0][3] = particleData[index].pos.x;
    pMatWorld[1][3] = particleData[index].pos.y;
    pMatWorld[2][3] = particleData[index].pos.z;
    
    pMatWorld = mul(motherMat,pMatWorld);

    //Ë°åÂ?óË®àÁÆ?-------------------------    
    GPUParticleInput outputMat;
    outputMat.worldMat = pMatWorld;
    outputMat.color = particleData[index].color.a;
    inputGPUParticleData.Append(outputMat);
}