#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/ExcuteIndirectHeader.hlsli"

cbuffer RootConstants : register(b0)
{
    float4 pos;
    uint vertMaxNum;
    uint bias;
};

struct OutputData
{
    float4 pos;
};

//更新
RWStructuredBuffer<float4> vertciesData : register(u0);
//出力
RWStructuredBuffer<OutputData> worldPosData : register(u1);

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

    //三角形を構成するレイ--------------------------------------------
    const int RAY_MAX_NUM = 3;
    const int RAY_POS_MAX_NUM = 2;
    float3 triangleRay[RAY_MAX_NUM][RAY_POS_MAX_NUM];
    triangleRay[0][0] = firstVertWorldPos.xyz;
    triangleRay[0][1] = secondVertWorldPos.xyz;
    triangleRay[1][0] = secondVertWorldPos.xyz;
    triangleRay[1][1] = thirdVertWorldPos.xyz;
    triangleRay[2][0] = thirdVertWorldPos.xyz;
    triangleRay[2][1] = firstVertWorldPos.xyz;
    //三角形を構成するレイ--------------------------------------------

    //重心座標
    float3 triangleCentralPos = (firstVertWorldPos.xyz + secondVertWorldPos.xyz + thirdVertWorldPos.xyz) / 3.0f;
 
    //パーティクルの配置--------------------------------------------
    const int PARTICLE_MAX_NUM = 50;
    const int PER_PARTICLE_MAX_NUM = PARTICLE_MAX_NUM / 3;
    for(int rayIndex = 0; rayIndex < RAY_MAX_NUM; ++rayIndex)
    {
        //一辺の長さ
        float3 distance = triangleRay[rayIndex][1] - triangleRay[rayIndex][0];

        for(int particleIndex = 0; particleIndex < PER_PARTICLE_MAX_NUM; ++particleIndex)
        {
            uint outputIndex = index * PARTICLE_MAX_NUM + rayIndex * PER_PARTICLE_MAX_NUM + particleIndex;
            float rate = RandVec3(outputIndex + 100,1,0).x;
            //始点算出
            float3 startPos = triangleRay[rayIndex][0] + distance * rate;
            //終点算出
            float3 endPos = triangleCentralPos;
            //一辺のとある位置から重心座標までの距離
            float3 resultDistance = endPos - startPos;

            //パーティクルの配置
            float3 resultPos;
            const int PARTICLE_MAX_BIAS = 100;
            const int RANDOM_NUMBER_BIAS = bias;
            
            if(RandVec3(outputIndex,PARTICLE_MAX_BIAS,0).x <= RANDOM_NUMBER_BIAS)
            {
                //エッジ周辺に偏らせる
                rate = RandVec3(outputIndex + 1000,10,0).x / 100.0f;
                resultPos = startPos + resultDistance * rate;
                worldPosData[outputIndex].pos.xyz = resultPos;
            }
            else
            {
                //面周辺に偏らせる
                rate = RandVec3(startPos.y * 10.0f + outputIndex + 10000,1,0).x;
                resultPos = startPos + resultDistance * rate;
                worldPosData[outputIndex].pos.xyz = resultPos;
            }
        }
    }
    //パーティクルの配置--------------------------------------------
}
