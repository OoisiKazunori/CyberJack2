#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/ExcuteIndirectHeader.hlsli"

struct OutputData
{
    matrix mat;
    float4 color;
};

RWStructuredBuffer<float3> worldPosColorArrayData : register(u0);
RWStructuredBuffer<OutputData> matrixData : register(u1);

cbuffer RootConstants : register(b0)
{    
    matrix scaleRotateBillboardMat;
    matrix viewProjection;
    uint posMaxNum;
};

[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = ThreadGroupIndex(groupId,groupIndex,groupThreadID,1024);
    if(posMaxNum <= index)
    {
        return;
    }
    //行列計算-------------------------
    matrix pMatWorld = scaleRotateBillboardMat;
    pMatWorld[0][3] = worldPosColorArrayData[index].x;
    pMatWorld[1][3] = worldPosColorArrayData[index].y;
    pMatWorld[2][3] = worldPosColorArrayData[index].z;
    //行列計算-------------------------

    //出力--------------------------------------------
    OutputData outputMat;
    outputMat.mat = mul(viewProjection,pMatWorld);     
    outputMat.color = float4(1,1,1,1);
    matrixData[index] = outputMat;
    //出力--------------------------------------------
}
