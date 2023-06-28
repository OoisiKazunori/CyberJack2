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
    
    //カメラからサンプリング地点までをレイマーチングで動かす定数で割り、サンプリング回数を求める。
    const int SAMPLING_COUNT = 16;
    float3 samplingDir = normalize(worldColor.xyz - cameraEyePos.m_eye);
    float samplingLength = length(cameraEyePos.m_eye - worldColor.xyz) / (float) SAMPLING_COUNT;
    float raymarchingBright = 0.0f;
    for (int counter = 0; counter < SAMPLING_COUNT; ++counter)
    {
                
        //ペイロード(再帰的に処理をするレイトレの中で値の受け渡しに使用する構造体)を宣言。
        Payload payloadData;
        payloadData.m_color = float3(0.0f, 0.0f, 0.0f); //色を真っ黒にしておく。レイを飛ばしてどこにもあたらなかった時に呼ばれるMissShaderが呼ばれたらそこで1を書きこむ。何かに当たったときに呼ばれるClosestHitShaderが呼ばれたらそこは影なので0を書き込む。
        
        //レイの設定
        RayDesc rayDesc;
        rayDesc.Origin = cameraEyePos.m_eye + (samplingDir * samplingLength) * counter; //レイの発射地点を設定。

        rayDesc.Direction = -lightData.m_dirLight.m_dir; //レイは光源に向かって飛ばす。
        rayDesc.TMin = 1.0f; //レイの最小値
        rayDesc.TMax = 300000.0f; //レイの最大値(カメラのFarみたいな感じ。)
    
        RAY_FLAG flag = RAY_FLAG_NONE; //レイのフラグ。背面カリングをしたり、AnyHitShaderを呼ばないようにする(軽量化)するときはここを設定する。
        flag |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    
        //レイを発射
        TraceRay(
        gRtScene, //TLAS
        flag,
        0xFF,
        0, //固定でよし。
        1, //固定でよし。
        1, //MissShaderのインデックス。RenderScene.cppでm_pipelineShadersにMissShaderを登録する際に2番目に影用のMissShaderを設定しているので、1にすると影用が呼ばれる。
        rayDesc,
        payloadData);
        
        //結果を保存。
        float progress = 1.0f / float(counter + 1);
        raymarchingBright = lerp(raymarchingBright, payloadData.m_color.x, progress);
        
    }
    const float3 FOGCOLOR_LIT = float3(1.0f, 1.0f, 1.0f);
    const float3 FOGCOLOR_UNLIT = float3(0.0f, 0.0f, 0.0f);
    
    float3 fogColor = lerp(FOGCOLOR_UNLIT, FOGCOLOR_LIT, raymarchingBright);
    const float FOG_DENSITY = 0.0001f;
    float absorb = exp(-length(cameraEyePos.m_eye - worldColor.xyz) * FOG_DENSITY);
    albedoColor.xyz = lerp(fogColor, albedoColor.xyz, absorb);
    
    //マテリアルのIDをもとに、反射屈折のレイを飛ばす。
    float4 final = float4(0, 0, 0, 0);
    SecondaryPass(materialInfo, normalColor, albedoColor, gRtScene, cameraEyePos, final);
    
    //合成の結果を入れる。
    finalColor[launchIndex.xy] = final;

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