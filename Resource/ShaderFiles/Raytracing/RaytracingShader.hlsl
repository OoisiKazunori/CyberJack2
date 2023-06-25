#include "RaytracingShaderHeader.hlsli"

//各リソース等
StructuredBuffer<uint> indexBuffer : register(t1, space1);
StructuredBuffer<Vertex> vertexBuffer : register(t2, space1);
Texture2D<float4> objectTexture : register(t0, space1);
//サンプラー
SamplerState smp : register(s0, space1);

//TLAS
RaytracingAccelerationStructure gRtScene : register(t0);

//カメラ座標用定数バッファ
struct CameraEyePosConstData
{
    float3 m_eye;
};
ConstantBuffer<CameraEyePosConstData> cameraEyePos : register(b0);

//GBuffer
Texture2D<float4> albedoMap : register(t1);
Texture2D<float4> normalMap : register(t2);
Texture2D<float4> materialMap : register(t3);
Texture2D<float4> worldMap : register(t4);

//出力先UAV
RWTexture2D<float4> finalColor : register(u0);


//RayGenerationシェーダー
[shader("raygeneration")]
void mainRayGen()
{

    uint2 launchIndex = DispatchRaysIndex().xy;
    
    float4 albedoColor = albedoMap[launchIndex];
    float4 normalColor = normalMap[launchIndex];
    float4 materialInfo = materialMap[launchIndex];
    float4 worldColor = worldMap[launchIndex];

    //ペイロードの設定
    Payload payloadData;
    payloadData.m_color = float3(1.0f, 1.0f, 1.0f);
    payloadData.m_rayID = RAY_SHADOW;
    
    //影用のレイをうつ。
    float bright = 1.0f;
    {
        float3 lightDir = normalize(float3(0.4f, -1.0f, 0.3f));
        
        //レイの設定
        RayDesc rayDesc;
        rayDesc.Origin = worldColor.xyz + normalColor.xyz;

        rayDesc.Direction = -lightDir;
        rayDesc.TMin = 0.0f;
        rayDesc.TMax = 300000.0f;
    
        RAY_FLAG flag = RAY_FLAG_NONE;
    
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
        
        bright = payloadData.m_color.x;
        
    }
    
    //アルベドにライトの色をかける。
    albedoColor.xyz *= clamp(bright, 0.3f, 1.0f);
    
    //レイのIDをみて、レイを打つかどうかを判断
    if (materialInfo.w != 0)
    {

        //レイの設定
        RayDesc rayDesc;
        rayDesc.Origin = worldColor.xyz + normalColor.xyz * 3.0f;

        rayDesc.Direction = refract(normalize(rayDesc.Origin - cameraEyePos.m_eye), normalColor.xyz, 0.01f);
        rayDesc.TMin = 0.0f;
        rayDesc.TMax = 300000.0f;
        
        payloadData.m_color = float3(1, 1, 1);
        payloadData.m_rayID = RAY_DEFAULT;
    
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
        

        //結果格納
        finalColor[launchIndex.xy] = float4(albedoColor.xyz, 1) * materialInfo.y;
        finalColor[launchIndex.xy] += float4((payloadData.m_color), 1) * (1.0f - materialInfo.y);
        
    }
    else
    {
        finalColor[launchIndex.xy] = albedoColor;
    }

}

//missシェーダー レイがヒットしなかった時に呼ばれるシェーダー
[shader("miss")]
void mainMS(inout Payload PayloadData)
{
    
    PayloadData.m_color = float3(1, 1, 1);

}

//シャドウ用missシェーダー
[shader("miss")]
void shadowMS(inout Payload payload)
{
    
    payload.m_color = float3(1, 1, 1);

}

//closesthitシェーダー レイがヒットした時に呼ばれるシェーダー
[shader("closesthit")]

    void mainCHS
    (inout
    Payload payload, MyAttribute
    attrib)
{
    
    if (payload.m_rayID == RAY_DEFAULT)
    {
    
        Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    
        float4 mainTexColor = objectTexture.SampleLevel(smp, vtx.uv, 1);
        payload.m_color = mainTexColor.xyz;
        
        //payload.m_color = float3(0,0,1);
           
    }
    else if (payload.m_rayID == RAY_SHADOW)
    {
        
        payload.m_color.x = 0.0f;
        
    }
    
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
        
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    
    
    payload.m_color = float3(0, 0, 1);
    
}