#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/GPUParticle.hlsli"

struct ColorData
{
    //X...一括光らせ、Y...高さで光らせる
    int2 lightData;
    float alpha;
};

//入力
RWStructuredBuffer<ParticleHitData> updateParticleData : register(u0);
RWStructuredBuffer<matrix> motherMatData : register(u1);
RWStructuredBuffer<float4> larpColorData : register(u2);
RWStructuredBuffer<ColorData> colorLightData : register(u3);

//出力
AppendStructuredBuffer<GPUParticleInput> inputGPUParticleData : register(u4);

cbuffer RootConstants : register(b0)
{
    matrix scaleRotaBillBoardMat;
};

[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = groupThreadID.x;
    index += 1024 * groupId.x;

    if(1500000 < index)
    {
        return;
    }


    ParticleHitData particleData = updateParticleData[index];
    float4 color = particleData.color;

    ColorData colorData = colorLightData[particleData.id];
    //色出力------------------------------------------------------------------
    bool TopFlag = particleData.pos.y <= colorData.lightData.y + 15.0f;
    bool ButtonFlag = colorData.lightData.y - 15.0f <= particleData.pos.y;

    const float4 LIGHT_COLOR = float4(0.7,0.7,0.7,1.0);
    //高さによる光らせ
    if(TopFlag && ButtonFlag)
    {
        color = LIGHT_COLOR;
    }
    //一括で光らせる
    else if(colorData.lightData.x)
    {
        color = LIGHT_COLOR;
    }
    color.a = colorData.alpha;

    larpColorData[index] = Larp(color,larpColorData[index],0.1f);
    //色出力------------------------------------------------------------------


    matrix worldMat = scaleRotaBillBoardMat;
    worldMat[0][3] = particleData.pos.x;
    worldMat[1][3] = particleData.pos.y;
    worldMat[2][3] = particleData.pos.z;
    worldMat = mul(motherMatData[particleData.id],worldMat);



    GPUParticleInput inputData;
    inputData.worldMat = worldMat;
    inputData.color = larpColorData[index];
    inputGPUParticleData.Append(inputData);
}
