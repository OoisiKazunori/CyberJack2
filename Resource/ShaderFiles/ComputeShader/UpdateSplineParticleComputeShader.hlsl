#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/ExcuteIndirectHeader.hlsli"

cbuffer RootConstants : register(b0)
{
    matrix scaleRotateBillboardMat;
    matrix viewProjection;
};

cbuffer LimitPosConstData : register(b1)
{
    uint limitIndexMaxNum;
};

struct UpdateData
{
    uint startIndex;
    float rate;
    float rateVel;
    float2 offset;
    float4 color;    
    float radius;
    float radiusRate;
};

struct OutputData
{
    matrix mat;
    float4 color;
};

//更新
RWStructuredBuffer<UpdateData> updateData : register(u0);
//出力
RWStructuredBuffer<OutputData> worldPosColorData : register(u1);

RWStructuredBuffer<float3> LimitPosData : register(u2);

[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = groupThreadID.x;
    index += 1024 * groupId.x;


    //スプライン曲線の挙動--------------------------------------------
    updateData[index].rate += updateData[index].rateVel;
    if(1.0f < updateData[index].rate)
    {
        updateData[index].rate = 0.0f;
        ++updateData[index].startIndex;
    }
    if(limitIndexMaxNum <= updateData[index].startIndex)
    {
        updateData[index].startIndex = 0;
    }
    //スプライン曲線の挙動--------------------------------------------

    //スプライン曲線上の位置を見る
    float4 splineData = SplinePosition(LimitPosData,updateData[index].startIndex,updateData[index].rate,limitIndexMaxNum);
    if(splineData.w)
    {
        updateData[index].color.a = 0;
    }
    else
    {
        updateData[index].color.a = 1;
    }

    float3 basePos = splineData.xyz;
	float3 offset = float3(updateData[index].offset.xy,0);

    float sinRate = sin(3.14f / 120.0f * updateData[index].radiusRate) * -(updateData[index].radius - 10);
    ++updateData[index].radiusRate;
	offset.x = cos(ConvertToRadian(RandVec3(index * 50,360,0).z)) * (updateData[index].radius + sinRate);
	offset.y = sin(ConvertToRadian(RandVec3(index * 50,360,0).z)) * (updateData[index].radius + sinRate);


    //スプライン曲線に沿った位置からオフセットを足した値がパーティクルの位置
    float3 particlePos = basePos + offset;


    //行列計算-------------------------
    matrix pMatWorld = scaleRotateBillboardMat;
    pMatWorld[0][3] = particlePos.x;
    pMatWorld[1][3] = particlePos.y;
    pMatWorld[2][3] = particlePos.z;
    //行列計算-------------------------

    //出力--------------------------------------------
    OutputData outputMat;
    outputMat.mat = mul(viewProjection,pMatWorld);     
    outputMat.color = updateData[index].color;
    worldPosColorData[index] = outputMat;
    //出力--------------------------------------------
}
