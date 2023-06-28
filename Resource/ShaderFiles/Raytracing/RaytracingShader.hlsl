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
ConstantBuffer<LightData> lightData : register(b1);

//GBuffer
Texture2D<float4> albedoMap : register(t1);
Texture2D<float4> normalMap : register(t2);
Texture2D<float4> materialMap : register(t3);
Texture2D<float4> worldMap : register(t4);

//出力先UAV
RWTexture2D<float4> finalColor : register(u0);

//レイトレ内で行うライティングパス
void LightingPass(inout float arg_bright, float4 arg_worldPosMap, float4 arg_normalMap)
{
    
    //ディレクションライト。
    if (lightData.m_dirLight.m_isActive && 0.1f < length(arg_normalMap.xyz))    //「ディレクションライトが有効だったら」 かつ 「現在のスクリーン座標の位置に法線が書き込まれていたら(何も書き込まれていないところからは影用のレイを飛ばさないようにするため。)」
    {
        
        //ペイロード(再帰的に処理をするレイトレの中で値の受け渡しに使用する構造体)を宣言。
        Payload payloadData;
        payloadData.m_color = float3(0.0f, 0.0f, 0.0f); //色を真っ黒にしておく。レイを飛ばしてどこにもあたらなかった時に呼ばれるMissShaderが呼ばれたらそこで1を書きこむ。何かに当たったときに呼ばれるClosestHitShaderが呼ばれたらそこは影なので0を書き込む。
        payloadData.m_rayID = RAY_DIR_SHADOW;               //レイのIDを設定。ClosestHitShaderでレイのIDによって処理を分けるため。
        
        //レイの設定
        RayDesc rayDesc;
        rayDesc.Origin = arg_worldPosMap.xyz;   //レイの発射地点を設定。

        rayDesc.Direction = -lightData.m_dirLight.m_dir;            //レイは光源に向かって飛ばす。
        rayDesc.TMin = 1.0f;        //レイの最小値
        rayDesc.TMax = 300000.0f;   //レイの最大値(カメラのFarみたいな感じ。)
    
        RAY_FLAG flag = RAY_FLAG_NONE;  //レイのフラグ。背面カリングをしたり、AnyHitShaderを呼ばないようにする(軽量化)するときはここを設定する。影用のレイなので背面カリングしちゃったら謎にライトが当たるので何も設定しない。
    
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
        
        //レイトレの結果の影情報を書き込む。
        arg_bright += payloadData.m_color.x;
        
    }
    
    //ポイントライト
    if (lightData.m_pointLight.m_isActive && 0.1f < length(arg_normalMap.xyz))    //「ポイントライトが有効だったら」 かつ 「現在のスクリーン座標の位置に法線が書き込まれていたら(何も書き込まれていないところからは影用のレイを飛ばさないようにするため。)」
    {
        
        //ペイロード(再帰的に処理をするレイトレの中で値の受け渡しに使用する構造体)を宣言。
        Payload payloadData;
        payloadData.m_color = float3(0.0f, 0.0f, 0.0f); //色を真っ黒にしておく。レイを飛ばしてどこにもあたらなかった時に呼ばれるMissShaderが呼ばれたらそこで1を書きこむ。何かに当たったときに呼ばれるClosestHitShaderが呼ばれたらそこは影なので0を書き込む。
        payloadData.m_rayID = RAY_POINT_SHADOW; //レイのIDを設定。ClosestHitShaderでレイのIDによって処理を分けるため。
        
        //レイの設定
        RayDesc rayDesc;
        rayDesc.Origin = arg_worldPosMap.xyz; //レイの発射地点を設定。
        
        //ポイントライトからのベクトルを求める。
        float3 lightDir = normalize(lightData.m_pointLight.m_pos - rayDesc.Origin);
        float distance = length(lightData.m_pointLight.m_pos - rayDesc.Origin);
        
        //距離がライトの最大影響範囲より大きかったらレイを飛ばさない。
        if (distance < lightData.m_pointLight.m_power)
        {

            rayDesc.Direction = lightDir; //レイは光源に向かって飛ばす。
            rayDesc.TMin = 1.0f; //レイの最小値
            rayDesc.TMax = distance; //レイの最大値(カメラのFarみたいな感じ。)
    
            RAY_FLAG flag = RAY_FLAG_NONE; //レイのフラグ。背面カリングをしたり、AnyHitShaderを呼ばないようにする(軽量化)するときはここを設定する。影用のレイなので背面カリングしちゃったら謎にライトが当たるので何も設定しない。
    
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
            
            //影が遮られていなかったら明るさを減衰させる。
            if (0 < payloadData.m_color.x)
            {
                
                //-------------------------------------------------------------------------------ここにジャックさんのライトの処理を書く。
            
                //ライト明るさの割合を求める。
                float brightRate = saturate(distance / lightData.m_pointLight.m_power);
        
                //仮で明るさにイージングをかける。
                payloadData.m_color.x = 1.0f - (brightRate * brightRate * brightRate);
                
            }
            
            
        
            //レイトレの結果の影情報を書き込む。
            arg_bright += payloadData.m_color.x;
            
        }
        
    }

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
    LightingPass(bright, worldColor, normalColor);
    
    //アルベドにライトの色をかける。
    albedoColor.xyz *= clamp(bright, 0.3f, 1.0f);
    
    //レイのIDをみて、レイを打つかどうかを判断
    if (materialInfo.w != 0 && 0.1f < length(normalColor.xyz))
    {

        //レイの設定
        RayDesc rayDesc;
        rayDesc.Origin = worldColor.xyz + normalColor.xyz * 3.0f;

        rayDesc.Direction = refract(normalize(rayDesc.Origin - cameraEyePos.m_eye), normalColor.xyz, 0.01f);
        rayDesc.TMin = 0.0f;
        rayDesc.TMax = 300000.0f;
        
        Payload payloadData;
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
    
    if (payload.m_rayID == RAY_DEFAULT)
    {
    
        float4 mainTexColor = objectTexture.SampleLevel(smp, vtx.uv, 1);
        payload.m_color = mainTexColor.xyz;
        
        //payload.m_color = float3(0,0,1);
           
    }
    else if (payload.m_rayID == RAY_DIR_SHADOW)
    {
        
        //このシェーダーに到達していたら影用のレイがオブジェクトに当たったということなので、payload.m_color.x(影情報)に黒を入れる。
        payload.m_color.x = 0.0f;
        
    }
    else if (payload.m_rayID == RAY_POINT_SHADOW)
    {    
        
        //このシェーダーに到達していたら影用のレイがオブジェクトに当たったということなので、payload.m_color.x(影情報)に黒を入れる。
        payload.m_color.x = 0.0f;
        
    }
    
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