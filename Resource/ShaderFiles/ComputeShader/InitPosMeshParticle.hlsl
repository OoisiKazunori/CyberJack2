#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/GPUParticle.hlsli"

//頂点情報
RWStructuredBuffer<float3> vertciesData : register(u0);
//出力
AppendStructuredBuffer<ParticleData> outputData : register(u1);
//RWStructuredBuffer<ParticleData> outputData : register(u1);


cbuffer CommonBuffer: register(b0)
{
    //x vertNum, y bias,z perTriangleNum,w faceCountNum
    uint4 meshData;
    float4 color;
    uint motherMatIndex;
}


float CalucurateTriangleArea(float3 P0,float3 P1,float3 P2)
{
    float3 p0p1Vec = P1 - P0;
    float3 p1p2Vec = P2 - P1;
    return length(cross(p0p1Vec,p1p2Vec)) / 2.0f;
}

float CalucurateUVW(float3 P0,float3 P1,float3 ATTACK_POINT,float TRIANGLE_AREA)
{
    float3 p0p1Vec = ATTACK_POINT - P0;
    float3 p1p2Vec = ATTACK_POINT - P1;
    float area = length(cross(p0p1Vec,p1p2Vec)) / 2.0f;   
    float rate = area / TRIANGLE_AREA;
    return rate;
}

[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = groupThreadID.x;
    index += 1024 * groupId.x;

    if(meshData.w <= index)
    {
        return;
    }

    //インデックス数以内なら処理する
    //三角形を構成するインデックスの指定--------------------------------------------
    uint firstVertIndex = index * 3;
    uint secondVertIndex = index * 3 + 1;
    uint thirdVertIndex = index * 3 + 2;

    uint uvFirstVertIndex = firstVertIndex;
    uint uvSecondVertIndex = secondVertIndex;
    uint uvThirdVertIndex = thirdVertIndex;
    //三角形を構成するインデックスの指定--------------------------------------------

    //頂点座標からワールド座標に変換後----------------------------------------------
    float3 firstVertWorldPos;
    float3 secondVertWorldPos;
    float3 thirdVertWorldPos;

    firstVertWorldPos = GetPos(vertciesData[firstVertIndex],float3(0,0,0)).xyz;
    secondVertWorldPos = GetPos(vertciesData[secondVertIndex],float3(0,0,0)).xyz;
    thirdVertWorldPos = GetPos(vertciesData[thirdVertIndex],float3(0,0,0)).xyz;
    //頂点座標からワールド座標に変換------------------------------------------------

    //三角形を構成するレイ--------------------------------------------
    const int RAY_MAX_NUM = 3;
    const int RAY_POS_MAX_NUM = 2;
    float3 triangleRay[RAY_MAX_NUM][RAY_POS_MAX_NUM];
    triangleRay[0][0] = firstVertWorldPos;
    triangleRay[0][1] = secondVertWorldPos;
    triangleRay[1][0] = secondVertWorldPos;
    triangleRay[1][1] = thirdVertWorldPos;
    triangleRay[2][0] = thirdVertWorldPos;
    triangleRay[2][1] = firstVertWorldPos;

    //三角形を構成するレイ--------------------------------------------

    //重心座標
    float3 triangleCentralPos = (firstVertWorldPos + secondVertWorldPos + thirdVertWorldPos) / 3.0f;

    //三角形の面積を計算
    float triangleArea = CalucurateTriangleArea(firstVertWorldPos,secondVertWorldPos,thirdVertWorldPos);

 
    //パーティクルの配置--------------------------------------------
    const int PARTICLE_MAX_NUM = meshData.z;
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
            const int RANDOM_NUMBER_BIAS = meshData.y;
            
            if(RandVec3(outputIndex,PARTICLE_MAX_BIAS,0).x <= RANDOM_NUMBER_BIAS)
            {
                //エッジ周辺に偏らせる
                rate = RandVec3(outputIndex + 1000,10,0).x / 100.0f;
                resultPos = startPos + resultDistance * rate;              
            }
            else
            {
                //面周辺に偏らせる
                rate = RandVec3(startPos.y * 10.0f + outputIndex + 10000,1,0).x;
                resultPos = startPos + resultDistance * rate;
            }

            ParticleData output;
            output.pos = resultPos;
            output.color = color;
            output.id = motherMatIndex;
            outputData.Append(output);            
            //outputData[outputIndex] = output;
        }
    }
    //パーティクルの配置--------------------------------------------
}