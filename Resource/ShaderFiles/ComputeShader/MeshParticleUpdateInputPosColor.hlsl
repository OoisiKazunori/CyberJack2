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
    float2 flash;
    float alpha;
};

struct UpdateData
{
    float3 pos;
    float4 color;
    float4 baseColor;
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

    float3 pos;
    pos.x = pMatWorld[0][3];
    pos.y = pMatWorld[1][3];
    pos.z = pMatWorld[2][3];
    //色出力------------------------------------------------------------------
    const float4 NORMAL_COLOR = worldPosColorArrayData[index].baseColor;
    const float4 FLASH_COLOR = worldPosColorArrayData[index].baseColor + float4(0.4f,0.4f,0.4f,0.0f);


    float lengthValue = 500.0f;
    bool TopFlag = pos.z <= flash.r + lengthValue;
    bool ButtonFlag = flash.r - lengthValue <= pos.z;
    //高さによる色
    if(TopFlag && ButtonFlag)
    {
        worldPosColorArrayData[index].color = FLASH_COLOR;
    }
    if(flash.g)
    {
        worldPosColorArrayData[index].color.xyz = worldPosColorArrayData[index].baseColor.xyz + float3(0.12f,0.12f,0.12f);
    }
    //段々通常の色に戻る
    if(NORMAL_COLOR.r < worldPosColorArrayData[index].color.r  &&
       NORMAL_COLOR.g < worldPosColorArrayData[index].color.g &&
       NORMAL_COLOR.b < worldPosColorArrayData[index].color.b)
    {
        worldPosColorArrayData[index].color.rgb -= 0.01f;
    }
    //色出力------------------------------------------------------------------


    //出力--------------------------------------------
    OutputData outputMat;
    outputMat.mat = mul(viewProjection,pMatWorld);     
    outputMat.color = worldPosColorArrayData[index].color;
    //float4(0.05,0.05,0.05,0)
    matrixData[index] = outputMat;
    //出力--------------------------------------------
}
