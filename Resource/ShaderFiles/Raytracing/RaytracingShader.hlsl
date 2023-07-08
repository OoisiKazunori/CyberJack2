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
ConstantBuffer<CameraEyePosConstData> cameraEyePos : register(b0);
ConstantBuffer<LightData> lightData : register(b1);
ConstantBuffer<RaymarchingParam> volumeFogData : register(b2);

//GBuffer
Texture2D<float4> albedoMap : register(t1);
Texture2D<float4> normalMap : register(t2);
Texture2D<float4> materialMap : register(t3);
Texture2D<float4> worldMap : register(t4);

//出力先UAV
RWTexture2D<float4> finalColor : register(u0);
RWTexture3D<float4> volumeNoiseTexture : register(u1);

//RayGenerationシェーダー
[shader("raygeneration")]
void mainRayGen()
{

    //現在のレイのインデックス。左上基準のスクリーン座標として使える。
    uint2 launchIndex = DispatchRaysIndex().xy;
    
    //GBufferから値を抜き取る。
    float4 albedoColor = albedoMap[launchIndex];
    float4 normalColor = normalMap[launchIndex];
    float4 materialInfo = materialMap[launchIndex];
    float4 worldColor = worldMap[launchIndex];
    
    //ライティングパスを行う。
    float bright = 0.0f;
    LightingPass(bright, worldColor, normalColor, lightData, gRtScene);
    
    //アルベドにライトの色をかける。
    albedoColor.xyz *= clamp(bright, 0.3f, 1.0f);
    
    //GodRayPass
    GodRayPass(worldColor, albedoColor, launchIndex, cameraEyePos, lightData, gRtScene, volumeNoiseTexture, volumeFogData);
    
    //マテリアルのIDをもとに、反射屈折のレイを飛ばす。
    float4 final = float4(0, 0, 0, 0);
    SecondaryPass(worldColor, materialInfo, normalColor, albedoColor, gRtScene, cameraEyePos, final);
    
    //合成の結果を入れる。
    finalColor[launchIndex.xy] = final;
    
    final.w = 1.0f;
    
    final.xyz = volumeNoiseTexture[uint3(launchIndex, 0.0f)].xyz;
    
    if (0 < length(final.xyz))
    {
        finalColor[launchIndex.xy] = final;
    }

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
    
    //このシェーダーに到達していたら影用のレイがオブジェクトに当たっていないということなので、payload.m_color.x(影情報)に白を入れる。
    payload.m_color = float3(1, 1, 1);

}

//closesthitシェーダー レイがヒットした時に呼ばれるシェーダー
[shader("closesthit")]

    void mainCHS
    (inout
    Payload payload, MyAttribute
    attrib)
{
    
    //ここにレイが当たった地点の頂点データとかが入っている。
    Vertex vtx = GetHitVertex(attrib, vertexBuffer, indexBuffer);
    
    //当たった位置のピクセルをサンプリングして、色をPayloadに入れる。
    float4 mainTexColor = objectTexture.SampleLevel(smp, vtx.uv, 1);
    payload.m_color = mainTexColor.xyz;
           
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