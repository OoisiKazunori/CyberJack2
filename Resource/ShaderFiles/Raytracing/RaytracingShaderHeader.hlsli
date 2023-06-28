
//円周率
static const float PI = 3.141592653589f;

//頂点情報
struct Vertex
{
    float3 pos;
    float3 normal;
    float2 uv;
    float3 tangent;
    float3 binormal;
};

//ペイロード
struct Payload
{
    float3 m_color; //色情報
    uint m_rayID;   //レイのID
};

struct MyAttribute
{
    float2 barys;
};

//ライト関係のデータ
struct DirLight
{
    float3 m_dir;
    int m_isActive;
};
struct PointLight
{
    float3 m_pos;
    float m_power;
    int3 m_pad;
    int m_isActive;
};
struct LightData
{
    DirLight m_dirLight;
    PointLight m_pointLight;
};

//カメラ用定数バッファ
struct CameraEyePosConstData
{
    float3 m_eye;
};

//barysを計算
inline float3 CalcBarycentrics(float2 Barys)
{
    return float3(1.0 - Barys.x - Barys.y, Barys.x, Barys.y);
}

//当たった位置の情報を取得する関数
Vertex GetHitVertex(MyAttribute attrib, StructuredBuffer<Vertex> vertexBuffer, StructuredBuffer<uint> indexBuffer)
{
    Vertex v = (Vertex) 0;
    float3 barycentrics = CalcBarycentrics(attrib.barys);
    uint vertexId = PrimitiveIndex() * 3; //Triangle List のため.

    float weights[3] =
    {
        barycentrics.x, barycentrics.y, barycentrics.z
    };

    for (int index = 0; index < 3; ++index)
    {
        uint vtxIndex = indexBuffer[vertexId + index];
        float w = weights[index];
        v.pos += vertexBuffer[vtxIndex].pos * w;
        v.normal += vertexBuffer[vtxIndex].normal * w;
        v.uv += vertexBuffer[vtxIndex].uv * w;
    }

    return v;
}

//レイトレ内で行うライティングパス
void LightingPass(inout float arg_bright, float4 arg_worldPosMap, float4 arg_normalMap, LightData arg_lightData, RaytracingAccelerationStructure arg_scene)
{
    
    //ディレクションライト。
    if (arg_lightData.m_dirLight.m_isActive && 0.1f < length(arg_normalMap.xyz))    //「ディレクションライトが有効だったら」 かつ 「現在のスクリーン座標の位置に法線が書き込まれていたら(何も書き込まれていないところからは影用のレイを飛ばさないようにするため。)」
    {
        
        //ペイロード(再帰的に処理をするレイトレの中で値の受け渡しに使用する構造体)を宣言。
        Payload payloadData;
        payloadData.m_color = float3(0.0f, 0.0f, 0.0f); //色を真っ黒にしておく。レイを飛ばしてどこにもあたらなかった時に呼ばれるMissShaderが呼ばれたらそこで1を書きこむ。
        
        //レイの設定
        RayDesc rayDesc;
        rayDesc.Origin = arg_worldPosMap.xyz; //レイの発射地点を設定。

        rayDesc.Direction = -arg_lightData.m_dirLight.m_dir; //レイは光源に向かって飛ばす。
        rayDesc.TMin = 1.0f; //レイの最小値
        rayDesc.TMax = 300000.0f; //レイの最大値(カメラのFarみたいな感じ。)
    
        RAY_FLAG flag = RAY_FLAG_NONE; //レイのフラグ。背面カリングをしたり、AnyHitShaderを呼ばないようにする(軽量化)するときはここを設定する。
        flag |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    
        //レイを発射
        TraceRay(
        arg_scene, //TLAS
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
    if (arg_lightData.m_pointLight.m_isActive && 0.1f < length(arg_normalMap.xyz))    //「ポイントライトが有効だったら」 かつ 「現在のスクリーン座標の位置に法線が書き込まれていたら(何も書き込まれていないところからは影用のレイを飛ばさないようにするため。)」
    {
        
        //ペイロード(再帰的に処理をするレイトレの中で値の受け渡しに使用する構造体)を宣言。
        Payload payloadData;
        payloadData.m_color = float3(0.0f, 0.0f, 0.0f); //色を真っ黒にしておく。レイを飛ばしてどこにもあたらなかった時に呼ばれるMissShaderが呼ばれたらそこで1を書きこむ。
        
        //レイの設定
        RayDesc rayDesc;
        rayDesc.Origin = arg_worldPosMap.xyz; //レイの発射地点を設定。
        
        //ポイントライトからのベクトルを求める。
        float3 lightDir = normalize(arg_lightData.m_pointLight.m_pos - rayDesc.Origin);
        float distance = length(arg_lightData.m_pointLight.m_pos - rayDesc.Origin);
        
        //距離がライトの最大影響範囲より大きかったらレイを飛ばさない。
        if (distance < arg_lightData.m_pointLight.m_power)
        {

            rayDesc.Direction = lightDir; //レイは光源に向かって飛ばす。
            rayDesc.TMin = 1.0f; //レイの最小値
            rayDesc.TMax = distance; //レイの最大値(カメラのFarみたいな感じ。)
    
            RAY_FLAG flag = RAY_FLAG_NONE; //レイのフラグ。背面カリングをしたり、AnyHitShaderを呼ばないようにする(軽量化)するときはここを設定する。
            flag |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    
            //レイを発射
            TraceRay(
            arg_scene, //TLAS
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
                float brightRate = saturate(distance / arg_lightData.m_pointLight.m_power);
        
                //仮で明るさにイージングをかける。
                payloadData.m_color.x = 1.0f - (brightRate * brightRate * brightRate);
                
            }
            
            
        
            //レイトレの結果の影情報を書き込む。
            arg_bright += payloadData.m_color.x;
            
        }
        
    }

}

void SecondaryPass(float4 arg_materialInfo, float4 arg_normalColor, float4 arg_albedoColor, RaytracingAccelerationStructure arg_scene, CameraEyePosConstData arg_cameraEyePos, inout float4 arg_finalColor)
{
        
    //レイのIDをみて、レイを打つかどうかを判断
    if (arg_materialInfo.w != 0 && 0.1f < length(arg_normalColor.xyz))
    {

        //レイの設定
        RayDesc rayDesc;
        rayDesc.Origin = arg_normalColor.xyz + arg_normalColor.xyz * 3.0f;

        rayDesc.Direction = refract(normalize(rayDesc.Origin - arg_cameraEyePos.m_eye), arg_normalColor.xyz, 0.01f);
        rayDesc.TMin = 0.0f;
        rayDesc.TMax = 300000.0f;
        
        Payload payloadData;
        payloadData.m_color = float3(1, 1, 1);
    
        RAY_FLAG flag = RAY_FLAG_NONE;
        flag |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES; //背面カリング
    
        //レイを発射
        TraceRay(
        arg_scene, //TLAS
        flag, //衝突判定制御をするフラグ
        0xFF, //衝突判定対象のマスク値
        0, //ray index
        1, //MultiplierForGeometryContrib
        0, //miss index
        rayDesc,
        payloadData);
        

        //結果格納
        arg_finalColor = float4(arg_albedoColor.xyz, 1) * arg_materialInfo.y;
        arg_finalColor += float4((payloadData.m_color), 1) * (1.0f - arg_materialInfo.y);
        
    }
    else
    {
        arg_finalColor = arg_albedoColor;
    }
    
}