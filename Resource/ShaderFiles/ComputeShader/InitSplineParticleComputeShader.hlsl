#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/ExcuteIndirectHeader.hlsli"

cbuffer RootConstants : register(b0)
{
    uint initMaxIndex;
};

struct OutputData
{
    uint startIndex;
    float rate;
	float rateVel;
    float2 offset;
    float4 color;
    float radius;
    float radiusRate;
};

//o—Í
RWStructuredBuffer<OutputData> worldPosData : register(u0);

[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = groupThreadID.x;
    index += 1024 * groupId.x;

    int startIndex = RandVec3(index,initMaxIndex,0).x;
    //ƒ‰ƒ“ƒ_ƒ€‚Å‹Èü‚Ì”ÍˆÍ“à‚Å—”‚ÅÀ•W‚ğŒˆ‚ß‚é
    float rate = RandVec3(index,1,0).y;
    
	float2 offset;
	offset.x = cos(ConvertToRadian(RandVec3(index * 50,360,0).z)) * RandVec3(index * 50,60,10).x;
	offset.y = sin(ConvertToRadian(RandVec3(index * 50,360,0).z)) * RandVec3(index * 150,60,10).y;

    worldPosData[index].startIndex = startIndex;
	worldPosData[index].rate = rate;
	worldPosData[index].rateVel = RandVec3(index,0.001f,0.009f).x;
	worldPosData[index].offset = offset;
	worldPosData[index].color = float4(RandVec3(index * 50,1,0),1);
    worldPosData[index].radiusRate = RandVec3(index * 50,1000,0).x;
    worldPosData[index].radius = RandVec3(index * 50,50,20).y;
}
