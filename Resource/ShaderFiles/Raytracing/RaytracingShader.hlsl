#include "RaytracingShaderHeader.hlsli"

//TLAS
RaytracingAccelerationStructure gRtScene : register(t0);

//各リソース等
StructuredBuffer<uint> indexBuffer : register(t0, space1);
StructuredBuffer<Vertex> vertexBuffer : register(t1, space1);
Texture2D<float4> objectTexture : register(t2, space1);
//サンプラー
SamplerState smp : register(s0, space1);

//GBuffer
Texture2D<float4> albedoMap : register(t1);
Texture2D<float4> normalMap : register(t2);
Texture2D<float4> materialMap : register(t3);
Texture2D<float4> worldMap : register(t4);
Texture2D<float4> sceneMap : register(t5);

//出力先UAV
RWTexture2D<float4> finalColor : register(u0);


//RayGenerationシェーダー
[shader("raygeneration")]
void mainRayGen()
{

    uint2 launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);
    
    float4 albedoColor = albedoMap[launchIndex];
    float4 normalColor = normalMap[launchIndex];
    float4 materialInfo = materialMap[launchIndex];
    float4 worldColor = worldMap[launchIndex];
    float4 sceneColor = sceneMap[launchIndex];

    //ペイロードの設定
    Payload payloadData;
    payloadData.m_color = float3(0, 0, 0);
    
    //レイのIDをみて、レイを打つかどうかを判断
    if (materialInfo.w != 0)
    {

    //レイの設定
    RayDesc rayDesc;
    rayDesc.Origin = worldColor.xyz + normalColor.xyz * 3.0f;

    rayDesc.Direction = normalColor.xyz;
    rayDesc.TMin = 0;
    rayDesc.TMax = 300000;
    
    RAY_FLAG flag = RAY_FLAG_NONE;
    flag |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES; //背面カリング
    
    //レイを発射
    TraceRay(
    gRtScene, //TLAS
    flag, //衝突判定制御をするフラグ
    0xFF, //衝突判定対象のマスク値
    0, //ray index
    1, //MultiplierForGeometryContrib
    0, //miss index
    rayDesc,
    payloadData);
        
        
    }

    //結果格納
    finalColor[launchIndex.xy] = sceneColor;
    finalColor[launchIndex.xy] += float4((payloadData.m_color), 1);

}

//missシェーダー レイがヒットしなかった時に呼ばれるシェーダー
[shader("miss")]
void mainMS(inout Payload PayloadData)
{
    
    PayloadData.m_color = float3(0, 0, 0);

}

//シャドウ用missシェーダー
[shader("miss")]
void shadowMS(inout Payload payload)
{
    
    payload.m_color = float3(0, 0, 0);

}

//closesthitシェーダー レイがヒットした時に呼ばれるシェーダー
[shader("closesthit")]

    void mainCHS
    (inout
    Payload payload, MyAttribute
    attrib)
{
    
    
    Vertex meshInfo[3];
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer, meshInfo);
    
    float4 mainTexColor = objectTexture.SampleLevel(smp, vtx.uv, 0);
    payload.m_color = mainTexColor.xyz;
    
    //payload.m_color = float3(0,0,1);
    
}

//影用CHS 使用していない。
[shader("closesthit")]

    void shadowCHS
    (inout
    Payload payload, MyAttribute
    attrib)
{
}

//AnyHitShader
[shader("anyhit")]

    void mainAnyHit
    (inout
    Payload payload, MyAttribute
    attrib)
{
        
    Vertex meshInfo[3];
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer, meshInfo);
    
    
    payload.m_color = float3(0, 0, 1);
    
}