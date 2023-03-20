#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/MeshCollision.hlsli"

cbuffer RootConstants : register(b0)
{
    float radius;
    uint xMax;
    uint xyMax;
};

//当たり判定座標
RWStructuredBuffer<MeshHitBox> hitBoxData : register(u1);

//BoundingBox内に配置された球同士の判定を取ってリンク付けする
[numthreads(1, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = ThreadGroupIndex(groupId,xMax,xyMax);
}
