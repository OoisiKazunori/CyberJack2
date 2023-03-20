#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/ExcuteIndirectHeader.hlsli"

struct OutputData
{
    float4 pos;
    float4 scale;
    float4 color;
};

struct UpdateData
{
    float4 pos;
    float4 vel;
    float4 color;
    uint timer;
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


    //円の大きさ、線の長さ、座標の決定--------------------------------------------
    int circleType = 2;
    float radius = 0.0f;
    float length = 1.0f;
    switch(circleType)
    {
    case 0:
    radius = 10.0f;
    length = Rand(index+5, index+30, 8.0f, 5.0f);
        break;
    case 1:
    radius = 20.0f;
    length = Rand(index+203, index+10, 5.0f, 3.0f);
        break;
    case 2:
    radius = 25.0f;
    length = Rand(index+9,index+12, 3.0f, 1.0f);
        break;
    }
    float x = 0.0f + cos(AngleToRadian(index)) * radius;
    float y = 0.0f + sin(AngleToRadian(index)) * radius;
    float3 outputPos = float3(x, y,  Rand(index+30, index-10, 700.0f, 0.0f));
    //円の大きさ、線の長さ、座標の決定--------------------------------------------

    
    //出力用-------------------------
    OutputData outputMat;
    outputMat.pos = float4(outputPos.xyz,0.0f);
     float scale = 0.1f;
    outputMat.scale = float4(float3(scale, scale, length),0.0f);
    outputMat.color = float4(0.6,0.6,0.6,0.6);
    matrixData[index] = outputMat;
    //出力用-------------------------

}