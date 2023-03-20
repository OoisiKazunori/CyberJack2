#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/ExcuteIndirectHeader.hlsli"

struct OutputData
{
    matrix mat;
    float4 color;
};

struct UpdateData
{
    float4 pos;
    float4 scale;
    float4 color;
};

cbuffer RootConstants : register(b0)
{
    matrix view;        
    matrix projection;
    matrix billBoard;
    float4 emmittPos;
    uint increSize;     //行列と色を出力する構造体のサイズ
    uint64_t gpuAddress; //行列と色を出力するバッファのGPUアドレス
    int seed;
};

//出力
RWStructuredBuffer<OutputData> matrixData : register(u0);
//更新
RWStructuredBuffer<UpdateData> updateData : register(u1);

static const int THREAD_MAX = 10;

[numthreads(THREAD_MAX, THREAD_MAX, THREAD_MAX)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    //uint index = (groupId.x * THREAD_MAX) + groupIndex;
    uint index = (groupThreadID.y * THREAD_MAX) + groupThreadID.x + ((THREAD_MAX * THREAD_MAX) * groupThreadID.z);
    index += (THREAD_MAX * THREAD_MAX * THREAD_MAX) * groupId.x;

    updateData[index].pos.z += -3.0f;
    if(updateData[index].pos.z <= -100.0f)
    {
        updateData[index].pos.z = 600.0f;
    }

    float3 outputPos = updateData[index].pos.xyz;
    float3 outputScale = updateData[index].scale.xyz;

    //行列計算-------------------------
    matrix pMatWorld = CalucurateWorldMat(outputPos,outputScale,float3(0,0,0));
    //行列計算-------------------------
    
    
    //出力用-------------------------
    OutputData outputMat;
    matrix lView = view;
    matrix lproj = projection;

    outputMat.mat = MatrixIdentity();
    outputMat.mat = mul(pMatWorld,outputMat.mat);
    outputMat.mat = mul(lView,    outputMat.mat);
    outputMat.mat = mul(lproj,    outputMat.mat);
    outputMat.color = updateData[index].color;
    matrixData[index] = outputMat;
    //出力用-------------------------

}