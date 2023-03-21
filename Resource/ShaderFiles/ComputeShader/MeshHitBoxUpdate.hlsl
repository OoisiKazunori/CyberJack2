#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/ExcuteIndirectHeader.hlsli"

cbuffer RootConstants : register(b0)
{
    matrix scaleRotateBillboardMat;
    matrix viewProjection;
    matrix motherMat;
};

struct HitBoxData
{
    float3 trianglePos[3];
    float3 normal;
};

//三角形の情報
RWStructuredBuffer<HitBoxData> initData : register(u0);
RWStructuredBuffer<HitBoxData> hitBoxData : register(u1);
struct OutputData
{
    matrix mat;
    float4 color;
};
RWStructuredBuffer<OutputData> outputData : register(u2);

[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = groupThreadID.x;
    index += 1024 * groupId.x;

	for(int trianglePosIndex = 0;trianglePosIndex < 3; ++trianglePosIndex)
    {
        //行列計算-------------------------
        matrix pMatWorld = scaleRotateBillboardMat;
        pMatWorld[0][3] = initData[index].trianglePos[trianglePosIndex].x;
        pMatWorld[1][3] = initData[index].trianglePos[trianglePosIndex].y;
        pMatWorld[2][3] = initData[index].trianglePos[trianglePosIndex].z;
        //行列計算-------------------------

        pMatWorld = mul(motherMat,pMatWorld);

        hitBoxData[index].trianglePos[trianglePosIndex].x = pMatWorld[0][3];
        hitBoxData[index].trianglePos[trianglePosIndex].y = pMatWorld[1][3];
        hitBoxData[index].trianglePos[trianglePosIndex].z = pMatWorld[2][3];

        //出力--------------------------------------------
        OutputData outputMat;
        outputMat.mat = mul(viewProjection, pMatWorld);
        outputMat.color = float4(0, 0, 0, 0);
        outputData[index * 3 + trianglePosIndex] = outputMat;
        //出力--------------------------------------------
    }

    //法線情報を得る
    float3 abVec = hitBoxData[index].trianglePos[1] - hitBoxData[index].trianglePos[0];
    float3 bcVec = hitBoxData[index].trianglePos[2] - hitBoxData[index].trianglePos[1];
    float3 normal = normalize(cross(abVec,bcVec));
    hitBoxData[index].normal = normal;

    outputData[index * 3 + 0].color = float4(normal.xyz,1.0f);
    outputData[index * 3 + 1].color = float4(normal.xyz,1.0f);
    outputData[index * 3 + 2].color = float4(normal.xyz,1.0f);
}