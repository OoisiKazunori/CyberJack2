#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/ExcuteIndirectHeader.hlsli"

struct UpdateData
{
    float4 pos;
    float4 color;
};

struct OutputData
{
    matrix mat;
    float4 color;
};

cbuffer RootConstants : register(b0)
{
    matrix viewProjection;
    matrix scaleRotateBillboardMat;
    //r...高さ,g...全体フラッシュ
    float2 flash;
};

//更新
RWStructuredBuffer<UpdateData> updateData : register(u0);
RWStructuredBuffer<UpdateData> baseUpdateData : register(u1);
//出力
RWStructuredBuffer<OutputData> matrixData : register(u2);

static const int THREAD_MAX = 10;

static const float4 FLASH_COLOR = float4(1.0f,1.0f,1.0f,1.0f);
static const float4 NORMAL_COLOR = float4(0.6f,0.6f,0.6f,0.6f);


[numthreads(THREAD_MAX, THREAD_MAX, THREAD_MAX)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = (groupThreadID.y * THREAD_MAX) + groupThreadID.x + ((THREAD_MAX * THREAD_MAX) * groupThreadID.z);
    index += (THREAD_MAX * THREAD_MAX * THREAD_MAX) * groupId.x;

    uint outPutIndex = index;

    //基本座標に戻るように補間をかける
    updateData[outPutIndex].pos.xyz = Larp(baseUpdateData[outPutIndex].pos.xyz,updateData[outPutIndex].pos.xyz,0.01f);

    //行列計算-------------------------
    matrix pMatWorld = scaleRotateBillboardMat;
    pMatWorld[0][3] = updateData[outPutIndex].pos.x;
    pMatWorld[1][3] = updateData[outPutIndex].pos.y;
    pMatWorld[2][3] = updateData[outPutIndex].pos.z;

    //行列計算-------------------------    
    OutputData outputMat;
    outputMat.mat = mul(viewProjection,pMatWorld);
    outputMat.color = updateData[outPutIndex].color;
    matrixData[outPutIndex] = outputMat;

}
