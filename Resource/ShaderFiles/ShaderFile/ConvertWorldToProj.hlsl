#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/GPUParticle.hlsli"

struct VertexBufferData
{
    float3 svpos;
    float3 normal;
    float2 uv;
    float3 tangent;
    float3 binormal;
};

float3 GetNormal(RWStructuredBuffer<VertexBufferData> vertex,uint index,uint2 offset)
{
    return cross(normalize(vertex[index + offset.x].svpos - vertex[index].svpos),normalize(vertex[index + offset.y].svpos - vertex[index].svpos));
}

void GenerateVertexData(inout RWStructuredBuffer<VertexBufferData> bufferData,int index,matrix worldMat)
{
    uint vertexIndex = index * 4;

    bufferData[index].svpos = float4(-0.5f, 0.5f, 0.0f, 1.0f);
    bufferData[index + 1].svpos = float4(-0.5f, -0.5f, 0.0f, 1.0f);
    bufferData[index + 2].svpos = float4(0.5f, 0.5f, 0.0f, 1.0f);
    bufferData[index + 3].svpos = float4(0.5f, -0.5f, 0.0f, 1.0f);

    bufferData[index].svpos =     mul(worldMat, float4(bufferData[index].svpos    , 1.0f));
    bufferData[index + 1].svpos = mul(worldMat, float4(bufferData[index + 1].svpos, 1.0f));
    bufferData[index + 2].svpos = mul(worldMat, float4(bufferData[index + 2].svpos, 1.0f));
    bufferData[index + 3].svpos = mul(worldMat, float4(bufferData[index + 3].svpos, 1.0f));

    bufferData[index].normal =     GetNormal(bufferData,index,uint2(1,2));
    bufferData[index + 1].normal = GetNormal(bufferData,index,uint2(1,2));
    bufferData[index + 2].normal = GetNormal(bufferData,index,uint2(1,2));
    bufferData[index + 3].normal = GetNormal(bufferData,index,uint2(1,2));
}

void GenerateIndexData(inout RWStructuredBuffer<uint> bufferData,int index)
{
    uint indeciesIndex = index * 6;
    if(index == 0)
    {
        bufferData[indeciesIndex] = indeciesIndex;
	    bufferData[indeciesIndex + 1] = indeciesIndex + 1;
	    bufferData[indeciesIndex + 2] = indeciesIndex + 2;
	    bufferData[indeciesIndex + 3] = indeciesIndex + 2;
	    bufferData[indeciesIndex + 4] = indeciesIndex + 1;
	    bufferData[indeciesIndex + 5] = indeciesIndex + 3;
    }
    else
    {
        uint offsetIndex = index * 4;
        bufferData[indeciesIndex] = offsetIndex;
	    bufferData[indeciesIndex + 1] = offsetIndex + 1;
	    bufferData[indeciesIndex + 2] = offsetIndex + 2;
	    bufferData[indeciesIndex + 3] = offsetIndex + 2;
	    bufferData[indeciesIndex + 4] = offsetIndex + 1;
	    bufferData[indeciesIndex + 5] = offsetIndex + 3;
    }
}


//�o��
ConsumeStructuredBuffer<GPUParticleInput> inputBuffer : register(u0);
AppendStructuredBuffer<GPUParticleInput> outputBuffer : register(u1);


RWStructuredBuffer<VertexBufferData> VertexBuffer : register(u2);
RWStructuredBuffer<uint> IndexBuffer : register(u3);


cbuffer Camera :register(b0)
{
    matrix viewProjMat;
}

//ワールド行列のバッファをプロジェクションに変換する
[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    GPUParticleInput inputData = inputBuffer.Consume();
    uint index = ThreadGroupIndex(groupId, groupIndex, groupThreadID, 1024);

    //試錐第カリング

    //頂点情報生成
    //GenerateVertexData(VertexBuffer,index,inputData.worldMat);
    //GenerateIndexData(IndexBuffer,index);

    GPUParticleInput outputData;
    outputData.worldMat = mul(viewProjMat,inputData.worldMat);
    outputData.color = inputData.color;
    outputBuffer.Append(inputData);
}
