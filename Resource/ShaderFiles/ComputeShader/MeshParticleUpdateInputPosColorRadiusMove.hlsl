#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/ExcuteIndirectHeader.hlsli"

struct OutputData
{
    matrix mat;
    float4 color;
};

cbuffer RootConstants : register(b0)
{    
    matrix scaleRotateBillboardMat;
    matrix viewProjection;
    matrix motherMat;
    float alpha;
};

struct UpdateData
{
    float3 pos;
    float4 color;
    float radius;
};

RWStructuredBuffer<UpdateData> worldPosColorArrayData : register(u0);
RWStructuredBuffer<OutputData> matrixData : register(u1);

[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = (groupThreadID.y * 1204) + groupThreadID.x + groupThreadID.z;
    index += 1024 * groupId.x;

    //行列計算-------------------------
    matrix pMatWorld = scaleRotateBillboardMat;
    pMatWorld[0][3] = worldPosColorArrayData[index].pos.x;
    pMatWorld[1][3] = worldPosColorArrayData[index].pos.y;
    pMatWorld[2][3] = worldPosColorArrayData[index].pos.z;

    pMatWorld = mul(motherMat,pMatWorld);
    //行列計算-------------------------


    //出力--------------------------------------------
    OutputData outputMat;
    outputMat.mat = mul(viewProjection,pMatWorld);     
    outputMat.color = worldPosColorArrayData[index].color;
    //float4(0.05,0.05,0.05,0)
    matrixData[index] = outputMat;
    //出力--------------------------------------------
}
