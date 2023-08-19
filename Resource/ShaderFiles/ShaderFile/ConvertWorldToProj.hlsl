#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/GPUParticle.hlsli"

//�o��
ConsumeStructuredBuffer<GPUParticleInput> inputBuffer : register(u0);
AppendStructuredBuffer<GPUParticleInput> outputBuffer : register(u1);

cbuffer Camera :register(b0)
{
    matrix viewProjMat;
}

//ワールド行列のバッファをプロジェクションに変換する
[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    GPUParticleInput inputData = inputBuffer.Consume();

    //試錐第カリング
    
    GPUParticleInput outputData;
    inputData.worldMat = mul(viewProjMat,inputData.worldMat);
    inputData.color = inputData.color;
    outputBuffer.Append(inputData);
}
