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

//空の色を取得。
float3 GetSkyColor(float3 arg_eyeVec)
{
    arg_eyeVec.y = max(arg_eyeVec.y, 0.0);
    float r = pow(1.0 - arg_eyeVec.y, 2.0);
    float g = 1.0 - arg_eyeVec.y;
    float b = 0.6 + (1.0 - arg_eyeVec.y) * 0.4;
    return float3(r, g, b);
}

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
    float4 final = float4(0, 0, 0, 1);
    SecondaryPass(worldColor, materialInfo, normalColor, albedoColor, gRtScene, cameraEyePos, final);
    
    //なにも描画されていないところでは空の色を取得。
    if (length(albedoColor.xyz) < 0.1f && length(worldColor.xyz) < 0.1f && length(normalColor.xyz) < 0.1f)
    {
       
        float2 dims = float2(DispatchRaysDimensions().xy);

        float2 d = (launchIndex.xy + 0.5) / dims.xy * 2.0 - 1.0;
        float aspect = dims.x / dims.y;

        float4 target = mul(cameraEyePos.m_projMat, float4(d.x, -d.y, 1, 1));
        float3 dir = mul(cameraEyePos.m_viewMat, float4(target.xyz, 0)).xyz;
        
        final.xyz = GetSkyColor(dir);
        
    }
    
    //合成の結果を入れる。
    finalColor[launchIndex.xy] = final;
  
}

//missシェーダー レイがヒットしなかった時に呼ばれるシェーダー
[shader("miss")]
void mainMS(inout Payload PayloadData)
{
    
    PayloadData.m_color = GetSkyColor(WorldRayDirection());

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