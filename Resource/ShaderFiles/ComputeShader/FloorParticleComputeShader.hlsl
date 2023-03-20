#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/ExcuteIndirectHeader.hlsli"

struct OutputData
{
    float4 pos;
};

//çXêV
RWStructuredBuffer<OutputData> updateData : register(u0);

static const int THREAD_MAX = 10;

[numthreads(THREAD_MAX, THREAD_MAX, THREAD_MAX)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = (groupThreadID.y * THREAD_MAX) + groupThreadID.x + ((THREAD_MAX * THREAD_MAX) * groupThreadID.z);
    index += (THREAD_MAX * THREAD_MAX * THREAD_MAX) * groupId.x;

    float4 initPos = float4(0,0,0,0);
    
    float4 generatePos = initPos + 
    float4
    (
        RandVec3(index,10000,-10000).x,
        0.0f,
        RandVec3(index,10000,100).z,
        0.0f
    );

    
    
    if(10000 <= index)
    {
       generatePos.y = -RandVec3(index + generatePos.x,950,900).y;
    }
    else
    {
        generatePos.y = RandVec3(index - generatePos.x,1200,1100).y;
    }

    updateData[index].pos = generatePos;
}