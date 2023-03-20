#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/GPUParticle.hlsli"

struct OutputData
{
    matrix mat;    
    float4 color;
};


cbuffer RootConstants : register(b0)
{    
    matrix viewProjection;
};

//入力
RWStructuredBuffer<GPUParticleInput> worldMatData : register(u0);

AppendStructuredBuffer<OutputData>drawData:register(u1);

//ワールド行列の座標や色からカリング処理を行い、描画用のバッファに変換する処理
[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = groupThreadID.x;
    index += 1024 * groupId.x;

    matrix worldMat = worldMatData[index].worldMat;
    float4 color = worldMatData[index].color;

    OutputData outputMat;
    outputMat.mat = mul(viewProjection,worldMat);
    outputMat.color = color;
    drawData.Append(outputMat);
}
