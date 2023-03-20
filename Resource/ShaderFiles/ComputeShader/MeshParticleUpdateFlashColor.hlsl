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
    float2 flash;
};

struct UpdateData
{
    float3 pos;
    float4 color;
};

RWStructuredBuffer<UpdateData> worldPosColorArrayData : register(u0);
RWStructuredBuffer<OutputData> matrixData : register(u1);

[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = (groupThreadID.y * 1204) + groupThreadID.x + groupThreadID.z;
    index += 1024 * groupId.x;

    float4 color;
    const float4 FLASH_COLOR = float4(1,1,1,1);
    const float4 NORMAL_COLOR = float4(0.6,0.6,0.6,0.6);

    //色出力------------------------------------------------------------------
    bool TopFlag = worldPosColorArrayData[index].pos.x <= flash.r + 2.0f;
    bool ButtonFlag = flash.r - 2.0f <= worldPosColorArrayData[index].pos.x;
    //高さによる色
    if(TopFlag && ButtonFlag)
    {
        worldPosColorArrayData[index].color = FLASH_COLOR;
    }
    if(flash.g)
    {
        worldPosColorArrayData[index].color = FLASH_COLOR;
    }
    //段々通常の色に戻る
    if( NORMAL_COLOR.r < worldPosColorArrayData[index].color.r  &&
        NORMAL_COLOR.g < worldPosColorArrayData[index].color.g &&
        NORMAL_COLOR.b < worldPosColorArrayData[index].color.b &&
        NORMAL_COLOR.a < worldPosColorArrayData[index].color.a)
    {
        worldPosColorArrayData[index].color -= 0.01f;
    }
    //色出力------------------------------------------------------------------


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
    outputMat.color = worldPosColorArrayData[index].color + float4(0.1,0.1,0.1,0);
    matrixData[index] = outputMat;
    //出力--------------------------------------------
}
