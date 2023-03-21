#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/ExcuteIndirectHeader.hlsli"

struct OutputData
{
    matrix mat;
    float4 color;
};

cbuffer RootConstants : register(b0)
{
    matrix view;
    matrix projection;
    matrix billBoard;
    float4 pos;
    uint vertMaxNum;
    uint indexMaxNum;
    uint dev;
};

//出力
AppendStructuredBuffer<OutputData> matrixData : register(u0);
//更新
RWStructuredBuffer<float4> vertciesData : register(u1);
RWStructuredBuffer<uint> indexData : register(u2);
RWStructuredBuffer<uint> countIndexData : register(u3);


static const int THREAD_MAX = 10;

[numthreads(THREAD_MAX, THREAD_MAX, THREAD_MAX)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = (groupThreadID.y * THREAD_MAX) + groupThreadID.x + ((THREAD_MAX * THREAD_MAX) * groupThreadID.z);
    index += (THREAD_MAX * THREAD_MAX * THREAD_MAX) * groupId.x;


    //頂点数以内なら処理する
    if(index < indexMaxNum && index + 1 < indexMaxNum)
    {
        uint nowVertIndex = indexData[index];
        uint nextVertIndex = indexData[index + 1];

        float3 defaltScale = float3(1,1,1);
        float3 defaltRota = float3(0,0,0);

        //ワールド座標の行列計算-------------------------
        matrix pMatWorld = CalucurateWorldMat(pos.xyz,defaltScale,defaltRota);
        //ワールド座標の行列計算-------------------------

        //頂点座標からワールド座標に変換後--------------------------------------
        float3 vertPos = float3(0,0,0);
        vertPos = vertciesData[nowVertIndex].xyz;
        matrix nowVertMatWorld = CalucurateWorldMat(vertPos,defaltScale,defaltRota);
        vertPos = vertciesData[nextVertIndex].xyz;
        matrix nextVertMatWorld = CalucurateWorldMat(vertPos,defaltScale,defaltRota);

        matrix nowWorldMat = mul(nowVertMatWorld,pMatWorld);
        matrix nextWorldMat = mul(nextVertMatWorld,pMatWorld);

        float4 nowVertWorldPos = float4(nowWorldMat[0].w,nowWorldMat[1].w,nowWorldMat[2].w,0.0f);
        float4 nextVertWorldPos = float4(nextWorldMat[0].w,nextWorldMat[1].w,nextWorldMat[2].w,0.0f);
        //頂点座標からワールド座標に変換--------------------------------------


        //お互いの距離から補間をかける--------------------------------------
        float4 distance = nextVertWorldPos - nowVertWorldPos;
        int absoluteValueOfDistance = CaluDistacne(nowVertWorldPos.xyz, nextVertWorldPos.xyz);
        //if(dev != 0)
        {
            absoluteValueOfDistance /= 2;
        }
        for(int i = 0; i < absoluteValueOfDistance; ++i)
        {           
            float rate = float(i + 1) / float(absoluteValueOfDistance);
            //出力用-------------------------
            float4 pos = nowVertWorldPos + distance * rate;
            matrix rMatTrans = CalucurateWorldMat(pos.xyz,defaltScale,defaltRota, billBoard);

            OutputData outputMat;
            outputMat.mat = CalucurateMat(rMatTrans, view, projection);
            outputMat.color = float4(1.0f,1.0f,0.0f,0.5f); 
            uint resultIndex = countIndexData[0] + i;
            matrixData.Append(outputMat);
            //出力用-------------------------
        }

        if(absoluteValueOfDistance == 0)
        {
            OutputData outputMat;
            countIndexData[0] += 1;
            uint resultIndex = countIndexData[0];
            outputMat.mat = MatrixIdentity();
            outputMat.color = float4(1.0f,1.0f,0.0f,0.5f);
            matrixData.Append(outputMat);
        }
        else
        {
            countIndexData[0] += absoluteValueOfDistance;
        }
        //お互いの距離から補間をかける--------------------------------------
    }
    else
    {
        OutputData outputMat;
        outputMat.color = float4(0.0f,0.0f,0.0f,0.0f);        
        matrix pMatWorld = CalucurateWorldMat(float3(0,0,0),float3(0,0,0),float3(0,0,0));
        outputMat.mat = CalucurateMat(pMatWorld,view,projection);
        matrixData.Append(outputMat);
    }
}
