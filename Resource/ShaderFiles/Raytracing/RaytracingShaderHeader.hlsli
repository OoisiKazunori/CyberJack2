
//円周率
static const float PI = 3.141592653589f;

//ミスシェーダーのインデックス
static const int MISS_DEFAULT = 0;
static const int MISS_LIGHTING = 1;

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
    float m_timer;
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

//レイを撃つ処理
void CastRay(inout Payload arg_payload, float3 arg_origin, float3 arg_dir, float arg_far, int arg_msIndex, RAY_FLAG arg_rayFlag, RaytracingAccelerationStructure arg_scene)
{
    //レイの設定
    RayDesc rayDesc;
    rayDesc.Origin = arg_origin; //レイの発射地点を設定。

    rayDesc.Direction = arg_dir; //レイの射出方向を設定。
    rayDesc.TMin = 1.0f; //レイの最小値
    rayDesc.TMax = arg_far; //レイの最大値(カメラのFarみたいな感じ。)
    
    //レイを発射
    TraceRay(
        arg_scene, //TLAS
        arg_rayFlag,
        0xFF,
        0, //固定でよし。
        1, //固定でよし。
        arg_msIndex, //MissShaderのインデックス。RenderScene.cppでm_pipelineShadersにMissShaderを登録している。
        rayDesc,
        arg_payload);
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
        
        //レイを撃つ
        CastRay(payloadData, arg_worldPosMap.xyz, -arg_lightData.m_dirLight.m_dir, 30000.0f, MISS_LIGHTING, RAY_FLAG_SKIP_CLOSEST_HIT_SHADER, arg_scene);
        
        //レイトレの結果の影情報を書き込む。
        arg_bright += payloadData.m_color.x;
        
    }
    
    //ポイントライト
    if (arg_lightData.m_pointLight.m_isActive && 0.1f < length(arg_normalMap.xyz))    //「ポイントライトが有効だったら」 かつ 「現在のスクリーン座標の位置に法線が書き込まれていたら(何も書き込まれていないところからは影用のレイを飛ばさないようにするため。)」
    {
        
        //ペイロード(再帰的に処理をするレイトレの中で値の受け渡しに使用する構造体)を宣言。
        Payload payloadData;
        payloadData.m_color = float3(0.0f, 0.0f, 0.0f); //色を真っ黒にしておく。レイを飛ばしてどこにもあたらなかった時に呼ばれるMissShaderが呼ばれたらそこで1を書きこむ。
        
        //ポイントライトからのベクトルを求める。
        float3 lightDir = normalize(arg_lightData.m_pointLight.m_pos - arg_worldPosMap.xyz);
        float distance = length(arg_lightData.m_pointLight.m_pos - arg_worldPosMap.xyz);
        
        //距離がライトの最大影響範囲より大きかったらレイを飛ばさない。
        if (distance < arg_lightData.m_pointLight.m_power)
        {
            
        
            //レイを撃つ
            CastRay(payloadData, arg_worldPosMap.xyz, lightDir, distance, MISS_LIGHTING, RAY_FLAG_SKIP_CLOSEST_HIT_SHADER, arg_scene);
            
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


void GodRayPass(float4 arg_worldColor, inout float4 arg_albedoColor, CameraEyePosConstData arg_cameraEyePos, LightData arg_lightData, RaytracingAccelerationStructure arg_scene)
{
    
    //カメラからサンプリング地点までをレイマーチングで動かす定数で割り、サンプリング回数を求める。
    const int SAMPLING_COUNT = 16;
    float3 samplingDir = normalize(arg_worldColor.xyz - arg_cameraEyePos.m_eye);
    float samplingLength = length(arg_cameraEyePos.m_eye - arg_worldColor.xyz) / (float) SAMPLING_COUNT;
    float raymarchingBright = 0.0f;
    for (int counter = 0; counter < SAMPLING_COUNT; ++counter)
    {
                
        //ペイロード(再帰的に処理をするレイトレの中で値の受け渡しに使用する構造体)を宣言。
        Payload payloadData;
        payloadData.m_color = float3(0.0f, 0.0f, 0.0f); //色を真っ黒にしておく。レイを飛ばしてどこにもあたらなかった時に呼ばれるMissShaderが呼ばれたらそこで1を書きこむ。何かに当たったときに呼ばれるClosestHitShaderが呼ばれたらそこは影なので0を書き込む。
        
        //ディレクションライトの処理
        if (arg_lightData.m_dirLight.m_isActive)
        {
        
            //レイを撃つ
            CastRay(payloadData, arg_cameraEyePos.m_eye + (samplingDir * samplingLength) * counter, -arg_lightData.m_dirLight.m_dir, 300000.0f, MISS_LIGHTING, RAY_FLAG_SKIP_CLOSEST_HIT_SHADER, arg_scene);
        
            //結果を保存。
            float progress = 1.0f / float(counter + 1);
            raymarchingBright = lerp(raymarchingBright, payloadData.m_color.x, progress);
            
        }
        //ポイントライトの処理
        float pointLightDistance = length((arg_cameraEyePos.m_eye + (samplingDir * samplingLength) * counter) - arg_worldColor.xyz);
        if (arg_lightData.m_pointLight.m_isActive && pointLightDistance < arg_lightData.m_pointLight.m_power)
        {
        
            //レイを撃つ
            CastRay(payloadData, arg_cameraEyePos.m_eye + (samplingDir * samplingLength) * counter, -arg_lightData.m_dirLight.m_dir, pointLightDistance, MISS_LIGHTING, RAY_FLAG_SKIP_CLOSEST_HIT_SHADER, arg_scene);
        
            //結果を保存。
            float progress = 1.0f / float(counter + 1);
            raymarchingBright = lerp(raymarchingBright, payloadData.m_color.x, progress);
            
        }
        
    }
    const float3 FOGCOLOR_LIT = float3(1.0f, 1.0f, 1.0f);
    const float3 FOGCOLOR_UNLIT = float3(0.0f, 0.0f, 0.0f);
    
    float3 fogColor = lerp(FOGCOLOR_UNLIT, FOGCOLOR_LIT, raymarchingBright);
    const float FOG_DENSITY = 0.0001f;
    float absorb = exp(-length(arg_cameraEyePos.m_eye - arg_worldColor.xyz) * FOG_DENSITY);
    arg_albedoColor.xyz = lerp(fogColor, arg_albedoColor.xyz, absorb);
    
}

void SecondaryPass(float4 arg_worldColor, float4 arg_materialInfo, float4 arg_normalColor, float4 arg_albedoColor, RaytracingAccelerationStructure arg_scene, CameraEyePosConstData arg_cameraEyePos, inout float4 arg_finalColor)
{
        
    //レイのIDをみて、レイを打つかどうかを判断
    if (arg_materialInfo.w != 0 && 0.1f < length(arg_normalColor.xyz))
    {
        
        Payload payloadData;
        payloadData.m_color = float3(1, 1, 1);
        
        //レイを撃つ
        float rayOrigin = arg_worldColor.xyz + arg_normalColor.xyz * 3.0f;
        CastRay(payloadData, rayOrigin, refract(normalize(rayOrigin - arg_cameraEyePos.m_eye), arg_normalColor.xyz, 0.01f), 300000.0f, MISS_DEFAULT, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, arg_scene);
        

        //結果格納
        arg_finalColor = float4(arg_albedoColor.xyz, 1) * arg_materialInfo.y;
        arg_finalColor += float4((payloadData.m_color), 1) * (1.0f - arg_materialInfo.y);
        
    }
    else
    {
        arg_finalColor = arg_albedoColor;
    }
    
}

//乱数を取得。
float3 Random3D(float3 arg_st)
{
    float3 seed = float3(dot(arg_st, float3(127.1f, 311.7f, 523.3f)), dot(arg_st, float3(269.5f, 183.3f, 497.5f)), dot(arg_st, float3(419.2f, 371.9f, 251.6f)));
    return -1.0f + 2.0f * frac(sin(seed) * 43758.5453123f);
}

//3Dグラディエントノイズ関数
float GradientNoise(float3 arg_st)
{
    float3 i = floor(arg_st);
    float3 f = frac(arg_st);

    //八つの隣接点の座標を求める
    float3 u = f * f * (3.0 - 2.0 * f);

    float a = dot(Random3D(i), f - float3(0, 0, 0));
    float b = dot(Random3D(i + float3(1, 0, 0)), f - float3(1, 0, 0));
    float c = dot(Random3D(i + float3(0, 1, 0)), f - float3(0, 1, 0));
    float d = dot(Random3D(i + float3(1, 1, 0)), f - float3(1, 1, 0));
    float e = dot(Random3D(i + float3(0, 0, 1)), f - float3(0, 0, 1));
    float f1 = dot(Random3D(i + float3(1, 0, 1)), f - float3(1, 0, 1));
    float g = dot(Random3D(i + float3(0, 1, 1)), f - float3(0, 1, 1));
    float h = dot(Random3D(i + float3(1, 1, 1)), f - float3(1, 1, 1));

    //ノイズ値を補間する
    float x1 = lerp(a, b, u.x);
    float x2 = lerp(c, d, u.x);
    float y1 = lerp(e, f1, u.x);
    float y2 = lerp(g, h, u.x);

    float xy1 = lerp(x1, x2, u.y);
    float xy2 = lerp(y1, y2, u.y);

    return lerp(xy1, xy2, u.z);
}

//3Dパーリンノイズ関数（風の表現付き）
float3 PerlinNoiseWithWind(float3 arg_st, int arg_octaves, float arg_persistence, float arg_lacunarity, float arg_windStrength, float arg_windSpeed, float arg_timer, float3 arg_worldPos, float arg_threshold)
{
    float amplitude = 1.0f;

    //風の影響を計算
    float3 windDirection = normalize(float3(1, 0, 0)); //風の方向を設定（この場合は (1, 0, 0) の方向）
    float3 windEffect = windDirection * arg_windStrength * (arg_timer * arg_windSpeed);

    //プレイヤーのワールド座標に基づくノイズ生成
    //float3 worldSpaceCoords = arg_st + arg_worldPos / 100.0f;
    float3 worldSpaceCoords = arg_st;

    float3 noiseValue = float3(0, 0, 0);

    for (int j = 0; j < 3; ++j)
    {
        float frequency = pow(2.0f, float(j));
        float localAmplitude = amplitude;
        float sum = 0.0f;
        float maxValue = 0.0f;
        
        for (int i = 0; i < arg_octaves; ++i)
        {
            sum += localAmplitude * GradientNoise((worldSpaceCoords + windEffect) * frequency + (arg_timer + windEffect.x)); //スムーズな時間変数と風の影響をノイズ関数に適用
            maxValue += localAmplitude;

            localAmplitude *= arg_persistence;
            frequency *= arg_lacunarity;
        }

        noiseValue[j] = (sum / maxValue + 1.0f) * 0.5f; //ノイズ値を0.0から1.0の範囲に再マッピング

        if (noiseValue[j] <= arg_threshold)
        {
            noiseValue[j] = 0.0f;
        }
    }

    return noiseValue;
}





//フラクタルノイズ
float FBM(float2 arg_st)
{
    float result = 0.0f;
    float amplitude = 1.0f;  //振幅

    for (int counter = 0; counter < 5; counter++)
    {
        result += amplitude * GradientNoise(float3(arg_st, 1.0f));
        amplitude *= 0.5f;   //振幅を減らす。こうするとノイズが細かくなっていく。
        arg_st *= 2.0f;      //周波数をあげていく。    
    }

    return result;
}

//ドメインワーピング
float3 DomainWarping(float2 arg_st, float arg_time)
{

    float3 color = float3(1.0f, 1.0f, 1.0f);

    //最初の引数
    float2 q = float2(0.0f, 0.0f);
    q.x = FBM(arg_st + float2(0.0f, 0.0f));
    q.y = FBM(arg_st + float2(1.0f, 1.0f));

    //最初の引数をさらに加工。
    float2 r = float2(0.0f, 0.0f);
    r.x = FBM(arg_st + (4.0f * q) + float2(1.7f, 9.2f) + (0.15f * arg_time));
    r.y = FBM(arg_st + (4.0f * q) + float2(8.3f, 2.8f) + (0.12f * arg_time));
    
    //色を求める。
    float3 mixColor1 = float3(0.8f, 0.35f, 0.12f);
    float3 mixColor2 = float3(0.3f, 0.75f, 0.69f);
    color = lerp(color, mixColor1, clamp(length(q), 0.0f, 1.0f));
    color = lerp(color, mixColor2, clamp(length(r), 0.0f, 1.0f));

    //三段階目のノイズを取得。
    float f = FBM(arg_st + 4.0f * r);

    //結果を組み合わせる。
    float coef = (f * f * f + (0.6f * f * f) + (0.5f * f)) * 10.0f;
    return color * coef;
    
}