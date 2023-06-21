#include "RaytracingShaderHeader.hlsli"

//TLAS
RaytracingAccelerationStructure gRtScene : register(t0);

//各リソース等
StructuredBuffer<uint> indexBuffer : register(t0, space1);
StructuredBuffer<Vertex> vertexBuffer : register(t1, space1);
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

    //レイの設定
    RayDesc rayDesc;
    rayDesc.Origin = float4(0, 0, 0, 1);
    rayDesc.Origin.xy += launchIndex * 2.0f;

    rayDesc.Direction = normalize(float3(0,0,1));
    rayDesc.TMin = 0;
    rayDesc.TMax = 300000;

    //ペイロードの設定
    Payload payloadData;
    payloadData.color_ = float3(0, 0, 0);
    
    RAY_FLAG flag = RAY_FLAG_NONE;
    flag |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;    //背面カリング
    
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

    //結果格納
   finalColor[launchIndex.xy] = float4((payloadData.color_), 1);

}

//missシェーダー レイがヒットしなかった時に呼ばれるシェーダー
[shader("miss")]
void mainMS(inout Payload PayloadData)
{
    
    PayloadData.color_ = float3(1, 0, 0);

}

//シャドウ用missシェーダー
[shader("miss")]
void shadowMS(inout Payload payload)
{
    
    payload.color_ = float3(1, 0, 0);

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
    
    payload.color_ = float3(0, 1, 0);
    
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
    
    
    payload.color_ = float3(0, 0, 1);
    
}