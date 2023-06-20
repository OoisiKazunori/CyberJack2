#include "RaytracingShaderHeader.hlsli"

//TLAS
RaytracingAccelerationStructure gRtScene : register(t0);

//各リソース等
StructuredBuffer<uint> indexBuffer : register(t0, space1);
StructuredBuffer<Vertex> vertexBuffer : register(t1, space1);
//サンプラー
SamplerState smp : register(s0, space1);

//出力先UAV
//RWTexture2D<float4> finalColor : register(u0);


//RayGenerationシェーダー
[shader("raygeneration")]
void mainRayGen()
{

    uint2 launchIndex = DispatchRaysIndex().xy;
    float2 dims = float2(DispatchRaysDimensions().xy);

    float2 d = (launchIndex.xy + 0.5) / dims.xy * 2.0 - 1.0;
    float aspect = dims.x / dims.y;

    //matrix mtxViewInv = gSceneParam.camera.mtxViewInv;
    //matrix mtxProjInv = gSceneParam.camera.mtxProjInv;

    //レイの設定
    RayDesc rayDesc;
    rayDesc.Origin = float4(0, 0, 0, 1);
    rayDesc.Origin.xy += launchIndex * 2.0f;

    //float4 target = mul(mtxProjInv, float4(d.x, -d.y, 1, 1));
    //float3 dir = mul(mtxViewInv, float4(target.xyz, 0)).xyz;

    rayDesc.Direction = normalize(float3(0,0,1));
    rayDesc.TMin = 0;
    rayDesc.TMax = 300000;

    //ペイロードの設定
    Payload payloadData;
    payloadData.color_ = float3(0, 0, 0);

    //TransRayに必要な設定を作成
    uint rayMask = 0xFF;
    
    RAY_FLAG flag = RAY_FLAG_NONE;
    flag |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
    
    //レイを発射
    TraceRay(
    gRtScene, //TLAS
    flag, //衝突判定制御をするフラグ
    rayMask, //衝突判定対象のマスク値
    0, //ray index
    1, //MultiplierForGeometryContrib
    0, //miss index
    rayDesc,
    payloadData);

    //結果格納
   //finalColor[launchIndex.xy] = float4((payloadData.color_), 1);

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