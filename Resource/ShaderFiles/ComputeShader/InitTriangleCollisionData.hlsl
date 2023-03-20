#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/ExcuteIndirectHeader.hlsli"

cbuffer RootConstants : register(b0)
{
    float4 pos;
};

struct OutputData
{
    float3 trianglePos[3];
    float3 normal;
};

//モデル頂点座標
RWStructuredBuffer<float4> vertciesData : register(u0);
//三角形と法線情報として出力
RWStructuredBuffer<OutputData> hitBoxData : register(u1);

[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = groupThreadID.x;
    index += 1024 * groupId.x;

    //インデックス数以内なら処理する
    //三角形を構成するインデックスの指定--------------------------------------------
    uint firstVertIndex = index * 3;
    uint secondVertIndex = index * 3 + 1;
    uint thirdVertIndex = index * 3 + 2;
    //三角形を構成するインデックスの指定--------------------------------------------

    //頂点座標からワールド座標に変換後----------------------------------------------
    float4 firstVertWorldPos = GetPos(vertciesData[firstVertIndex].xyz,pos.xyz);
    float4 secondVertWorldPos = GetPos(vertciesData[secondVertIndex].xyz,pos.xyz);
    float4 thirdVertWorldPos = GetPos(vertciesData[thirdVertIndex].xyz,pos.xyz);
    //頂点座標からワールド座標に変換------------------------------------------------

    //三角形の情報を得る
    hitBoxData[index].trianglePos[0] = firstVertWorldPos.xyz;
    hitBoxData[index].trianglePos[1] = secondVertWorldPos.xyz;
    hitBoxData[index].trianglePos[2] = thirdVertWorldPos.xyz;

    //法線情報を得る
    float3 abVec = secondVertWorldPos.xyz - firstVertWorldPos.xyz;
    float3 bcVec = thirdVertWorldPos.xyz - secondVertWorldPos.xyz;    
    float3 normal = normalize(abVec * bcVec);
    hitBoxData[index].normal = normal;
}