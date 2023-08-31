#include "RaytracingNoiseHeader.hlsli"

//円周率
static const float PI = 3.141592653589f;

//ミスシェーダーのインデックス
static const int MISS_DEFAULT = 0;
static const int MISS_LIGHTING = 1;
static const int MISS_CHECKHIT = 2;

//マテリアルの種類
static const int MATERIAL_NONE = 0;
static const int MATERIAL_REFLECT = 1;
static const int MATERIAL_REFRACT = 2;
static const int MATERIAL_SEA = 3;

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
    float3 m_emissive; //輝度
    uint m_rayID; //レイのID
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
    matrix m_viewMat;
    matrix m_projMat;
    float3 m_eye;
    float m_timer;
};

//ボリュームフォグ用定数バッファ
struct RaymarchingParam
{
    float3 m_pos; //ボリュームテクスチャのサイズ
    float m_gridSize; //サンプリングするグリッドのサイズ
    float3 m_color; //フォグの色
    float m_wrapCount; //サンプリング座標がはみ出した際に何回までWrapするか
    float m_sampleLength; //サンプリング距離
    float m_density; //濃度係数
    int m_isSimpleFog;
    int m_isActive;
};

//OnOffデバッグ
struct DebugRaytracingParam
{
    int m_debugReflection;
    int m_debugShadow;
    float m_sliderRate;
    float m_skyFacter;
};

//OnOffデバッグ
struct DebugSeaParam
{
    float m_freq;
    float m_amp;
    float m_choppy;
    float m_seaSpeed;
};

//衝撃波
struct ShockWave
{
    float3 m_pos;
    float m_radius;
    float m_power;
    int m_isActive;
    float m_facter;
    float m_pad;
};
struct ShockWaveParam
{
    ShockWave m_shockWave[8];
};



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
ConstantBuffer<DebugRaytracingParam> debugRaytracingData : register(b2);
ConstantBuffer<DebugSeaParam> debugSeaData : register(b3);
ConstantBuffer<ShockWaveParam> shockWaveData : register(b4);

//GBuffer
Texture2D<float4> albedoMap : register(t1);
Texture2D<float4> normalMap : register(t2);
Texture2D<float4> materialMap : register(t3);
Texture2D<float4> worldMap : register(t4);
Texture2D<float4> emissiveMap : register(t5);

//出力先UAV
RWTexture2D<float4> finalColor : register(u0);
RWTexture3D<float4> volumeNoiseTexture : register(u1);
RWTexture2D<float4> lensFlareTexture : register(u2);
RWTexture2D<float4> emissiveTexture : register(u3);


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
void CastRay(inout Payload arg_payload, float3 arg_origin, float3 arg_dir, float arg_far, int arg_msIndex, RAY_FLAG arg_rayFlag, RaytracingAccelerationStructure arg_scene, uint arg_instanceMask)
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
        arg_instanceMask,
        0, //固定でよし。
        1, //固定でよし。
        arg_msIndex, //MissShaderのインデックス。RenderScene.cppでm_pipelineShadersにMissShaderを登録している。
        rayDesc,
        arg_payload);
}

//レイトレ内で行うライティングパス
void LightingPass(inout float arg_bright, float4 arg_worldPosMap, float4 arg_normalMap, LightData arg_lightData, uint2 arg_launchIndex, DebugRaytracingParam arg_debugRaytracingData, RaytracingAccelerationStructure arg_scene, bool arg_isFar)
{
    
    //ディレクションライト。
    if (arg_lightData.m_dirLight.m_isActive && 0.1f < length(arg_normalMap.xyz))    //「ディレクションライトが有効だったら」 かつ 「現在のスクリーン座標の位置に法線が書き込まれていたら(何も書き込まれていないところからは影用のレイを飛ばさないようにするため。)」
    {
        
        //ペイロード(再帰的に処理をするレイトレの中で値の受け渡しに使用する構造体)を宣言。
        Payload payloadData;
        payloadData.m_emissive = float3(0.0f, 0.0f, 0.0f);
        payloadData.m_color = float3(0.0f, 0.0f, 0.0f); //色を真っ黒にしておく。レイを飛ばしてどこにもあたらなかった時に呼ばれるMissShaderが呼ばれたらそこで1を書きこむ。
        payloadData.m_rayID = 1;
        
        //レイを撃つ
        bool isDebug = arg_debugRaytracingData.m_debugShadow == 1 && arg_launchIndex.x < arg_debugRaytracingData.m_sliderRate;
        if (isDebug || arg_isFar)
        {
            payloadData.m_color = float3(1, 1, 1);
        }
        else
        {
            CastRay(payloadData, arg_worldPosMap.xyz, -arg_lightData.m_dirLight.m_dir, 1000.0f, MISS_LIGHTING, RAY_FLAG_NONE, arg_scene, 0x01);
        }
        
        //ライトのベクトルと法線から明るさを計算する。
        float bright = saturate(dot(arg_normalMap.xyz, -arg_lightData.m_dirLight.m_dir));
        
        //レイトレの結果の影情報を書き込む。
        arg_bright += payloadData.m_color.x * bright;
        
    }
    
    //ポイントライト
    if (arg_lightData.m_pointLight.m_isActive && 0.1f < length(arg_normalMap.xyz))    //「ポイントライトが有効だったら」 かつ 「現在のスクリーン座標の位置に法線が書き込まれていたら(何も書き込まれていないところからは影用のレイを飛ばさないようにするため。)」
    {
        
        //ペイロード(再帰的に処理をするレイトレの中で値の受け渡しに使用する構造体)を宣言。
        Payload payloadData;
        payloadData.m_emissive = float3(0.0f, 0.0f, 0.0f);
        payloadData.m_color = float3(0.0f, 0.0f, 0.0f); //色を真っ黒にしておく。レイを飛ばしてどこにもあたらなかった時に呼ばれるMissShaderが呼ばれたらそこで1を書きこむ。
        
        //ポイントライトからのベクトルを求める。
        float3 lightDir = normalize(arg_lightData.m_pointLight.m_pos - arg_worldPosMap.xyz);
        float distance = length(arg_lightData.m_pointLight.m_pos - arg_worldPosMap.xyz);
        
        //距離がライトの最大影響範囲より大きかったらレイを飛ばさない。
        if (distance < arg_lightData.m_pointLight.m_power)
        {
            
        
            //レイを撃つ
            CastRay(payloadData, arg_worldPosMap.xyz, lightDir, distance, MISS_LIGHTING, RAY_FLAG_SKIP_CLOSEST_HIT_SHADER, arg_scene, 0x01);
            
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



//空の色を取得。
float3 GetSkyColor(float3 arg_eyeVec)
{
    arg_eyeVec.y = max(arg_eyeVec.y, 0.0f);
    float r = pow(1.0f - arg_eyeVec.y, 2.0f);
    float g = 1.0f - arg_eyeVec.y;
    float b = 0.6f + (1.0f - arg_eyeVec.y) * 0.4f;
    return float3(r, g, b);
}

//ライティングに関する関数
float Diffuse(float3 arg_normal, float3 arg_light, float arg_position)
{
    return pow(dot(arg_normal, arg_light) * 0.4f + 0.6f, arg_position);
}
float Specular(float3 arg_normal, float3 arg_light, float3 arg_eye, float arg_specular)
{
    float nrm = (arg_specular + 8.0f) / (PI * 8.0f);
    return pow(max(dot(reflect(arg_eye, arg_normal), arg_light), 0.0f), arg_specular) * nrm;
}

//海の色を取得
static const float3 SEA_BASE = float3(0.1f, 0.19f, 0.22f); //謎。 海が完成したら動かしてみて何かを判断する。
static const float3 SEA_WATER_COLOR = float3(0.8f, 0.9f, 0.6f); //名前的に水の色
float3 GetSeaColor(float3 arg_position, float3 arg_normal, float3 arg_light, float3 arg_rayDir, float3 arg_dist /*arg_position - レイの原点*/)
{
    //海に関する定数 実装出来たらこれらを定数バッファに入れて変えられるようにする。
    const float SEA_HEIGHT = 0.6f; //海の限界の高さ？
    
    //フレネルの計算で反射率を求める。 http://marupeke296.com/DXPS_PS_No7_FresnelReflection.html
    float fresnel = clamp(1.0f - dot(arg_normal, -arg_rayDir), 0.0f, 1.0f);
    fresnel = pow(fresnel, 3.0f) * 0.65f;

    //反射、屈折した場合の色を求める。
    float3 reflected = GetSkyColor(reflect(arg_rayDir, arg_normal));
    float3 refracted = SEA_BASE + Diffuse(arg_normal, arg_light, 80.0f) * SEA_WATER_COLOR * 0.12f; //海の色 この先にオブジェクトがある場合、その距離に応じてその色を補間する。

    //フレネルの計算で得られた反射率から色を補間する。
    float3 color = lerp(refracted, reflected, fresnel);

    //減衰率を求める。
    float atten = max(1.0f - dot(arg_dist, arg_dist) * 0.001f, 0.0f);
    color += SEA_WATER_COLOR * (arg_position.y - SEA_HEIGHT) * 0.18f * atten; //波の高さによって色を変えてる？ここを調整すれば白くできるかも？

    //スペキュラを求めて光沢を出す！
    color += float3(float3(1.0f, 1.0f, 1.0f) * Specular(arg_normal, arg_light, arg_rayDir, 5.0f));

    return color;
}

//全部の要素が既定の値以内に収まっているか。
bool IsInRange(float3 arg_value, float arg_range, float arg_wrapCount)
{
    
    bool isInRange = arg_value.x / arg_range <= arg_wrapCount && arg_value.y / arg_range <= arg_wrapCount && arg_value.z / arg_range <= arg_wrapCount;
    isInRange &= 0 < arg_value.x && 0 < arg_value.y && 0 < arg_value.z;
    return isInRange;
}

void SecondaryPass(float3 arg_viewDir, inout float4 arg_emissiveColor, float4 arg_worldColor, float4 arg_materialInfo, float4 arg_normalColor, float4 arg_albedoColor, RaytracingAccelerationStructure arg_scene, CameraEyePosConstData arg_cameraEyePos, inout float4 arg_finalColor)
{
        
    //レイのIDをみて、レイを打つかどうかを判断
    if (arg_materialInfo.w == MATERIAL_REFRACT && 0.1f < length(arg_normalColor.xyz))
    {
        
        Payload payloadData;
        payloadData.m_emissive = float3(0.0f, 0.0f, 0.0f);
        payloadData.m_color = float3(1, 1, 1);
        payloadData.m_rayID = 0;
        
        //レイを撃つ
        float3 rayOrigin = arg_worldColor.xyz + arg_normalColor.xyz * 3.0f;
        CastRay(payloadData, rayOrigin, refract(arg_viewDir, arg_normalColor.xyz, 0.1f), 100.0f, MISS_DEFAULT, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, arg_scene, 0xFF);
        
        //結果格納
        arg_finalColor = float4(arg_albedoColor.xyz, 1) * arg_materialInfo.y;
        arg_finalColor += float4((payloadData.m_color), 1) * (1.0f - arg_materialInfo.y);
        arg_emissiveColor.xyz += payloadData.m_emissive;
        
    }
    else if (arg_materialInfo.w == MATERIAL_REFLECT && 0.1f < length(arg_normalColor.xyz))
    {
        
        Payload payloadData;
        payloadData.m_emissive = float3(0.0f, 0.0f, 0.0f);
        payloadData.m_color = float3(1, 1, 1);
        payloadData.m_rayID = 0;
        
        //レイを撃つ
        float3 rayOrigin = arg_worldColor.xyz + arg_normalColor.xyz * 3.0f;
        CastRay(payloadData, rayOrigin, reflect(arg_viewDir, arg_normalColor.xyz), 100.0f, MISS_DEFAULT, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, arg_scene, 0xFF);
        
        //結果格納
        arg_finalColor = float4(arg_albedoColor.xyz, 1) * arg_materialInfo.y;
        arg_finalColor += float4((payloadData.m_color), 1) * (1.0f - arg_materialInfo.y);
        arg_emissiveColor.xyz += payloadData.m_emissive;
        
    }
    else if (arg_materialInfo.w == MATERIAL_SEA && 0.1f < length(arg_normalColor.xyz))
    {
        
        Payload refractionColor;
        refractionColor.m_emissive = float3(0.0f, 0.0f, 0.0f);
        refractionColor.m_color = float3(1, 1, 1);
        refractionColor.m_rayID = 2;
        Payload reflectionColor;
        reflectionColor.m_emissive = float3(0.0f, 0.0f, 0.0f);
        reflectionColor.m_color = float3(1, 1, 1);
        reflectionColor.m_rayID = 2;
        
        //レイを撃つ
        float3 rayOrigin = arg_worldColor.xyz;
        CastRay(refractionColor, rayOrigin, refract(arg_viewDir, arg_normalColor.xyz, 0.1f), 500.0f, MISS_CHECKHIT, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, arg_scene, 0xFF);
        CastRay(reflectionColor, rayOrigin, reflect(arg_viewDir, arg_normalColor.xyz), 500.0f, MISS_CHECKHIT, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, arg_scene, 0xFF);
        
        //レイが当たったか当たっていないかで色を変える。
        if (refractionColor.m_color.x < 0)
        {
            refractionColor.m_color = arg_albedoColor.xyz;
        }
        if (reflectionColor.m_color.x < 0)
        {
            reflectionColor.m_color = float3(0, 0, 0);
        }
        
        //海の色の割合
        float perOfSeaColor = (1.0f - arg_materialInfo.y);
        
        //結果格納
        arg_finalColor = float4(arg_albedoColor.xyz, 1) * arg_materialInfo.y;
        arg_finalColor += float4((refractionColor.m_color), 1) * (perOfSeaColor / 2.0f);
        arg_finalColor += float4((reflectionColor.m_color), 1) * (perOfSeaColor / 2.0f);
        
        arg_emissiveColor.w = 1.0f;
        arg_emissiveColor.xyz += refractionColor.m_emissive;
        arg_emissiveColor.xyz += reflectionColor.m_emissive;
        
    }
    else
    {
        arg_finalColor = arg_albedoColor;
    }
}



float3 IntersectionPos(float3 Dir, float3 A, float Radius)
{
    float b = dot(A, Dir);
    float c = dot(A, A) - Radius * Radius;
    float d = max(b * b - c, 0.0f);

    return A + Dir * (-b + sqrt(d));
}
float Scale(float FCos)
{
    float x = 1.0f - FCos;
    return 0.25f * exp(-0.00287f + x * (0.459f + x * (3.83f + x * (-6.80f + x * 5.25f))));
}

//大気散乱
float3 AtmosphericScattering(float3 pos, inout float3 mieColor)
{
    
    //レイリー散乱定数
    float kr = 0.0025f;
    //ミー散乱定数
    float km = 0.005f;

    //大気中の線分をサンプリングする数。
    float fSamples = 2.0f;

    //謎の色 色的には薄めの茶色
    float3 three_primary_colors = float3(0.68f, 0.55f, 0.44f);
    //光の波長？
    float3 v3InvWaveLength = 1.0f / pow(three_primary_colors, 4.0f);

    //大気圏の一番上の高さ。
    float fOuterRadius = 10250.0f;
    //地球全体の地上の高さ。
    float fInnerRadius = 10200.0f;

    //太陽光の強さ？
    float fESun = 10.0f;
    //太陽光の強さにレイリー散乱定数をかけてレイリー散乱の強さを求めている。
    float fKrESun = kr * fESun;
    //太陽光の強さにミー散乱定数をかけてレイリー散乱の強さを求めている。
    float fKmESun = km * fESun;

    //レイリー散乱定数に円周率をかけているのだが、限りなく0に近い値。
    float fKr4PI = kr * 4.0f * PI;
    //ミー散乱定数に円周率をかけているのだが、ミー散乱定数は0なのでこれの値は0。
    float fKm4PI = km * 4.0f * PI;

    //地球全体での大気の割合。
    float fScale = 1.0f / (fOuterRadius - fInnerRadius);
    //平均大気密度を求める高さ。
    float fScaleDepth = 0.35f;
    //地球全体での大気の割合を平均大気密度で割った値。
    float fScaleOverScaleDepth = fScale / fScaleDepth;

    //散乱定数を求める際に使用する値。
    float g = -0.999f;
    //散乱定数を求める際に使用する値を二乗したもの。なぜ。
    float g2 = g * g;

    //当たった天球のワールド座標
    float3 worldPos = normalize(pos) * fOuterRadius;
    worldPos = IntersectionPos(normalize(worldPos), float3(0.0, fInnerRadius, 0.0), fOuterRadius);

    //カメラ座標 元計算式だと中心固定になってしまっていそう。
    float3 v3CameraPos = float3(0.0, fInnerRadius + 1.0f, 0.0f);

    //ディレクショナルライトの場所を求める。
    float3 dirLightPos = -lightData.m_dirLight.m_dir * 15000.0f;

    //ディレクショナルライトへの方向を求める。
    float3 v3LightDir = normalize(dirLightPos - worldPos);

    //天球上頂点からカメラまでのベクトル(光が大気圏に突入した点からカメラまでの光のベクトル)
    float3 v3Ray = worldPos - v3CameraPos;

    //大気に突入してからの点とカメラまでの距離。
    float fFar = length(v3Ray);

    //正規化された拡散光が来た方向。
    v3Ray /= fFar;

    //サンプリングする始点座標 資料だとAの頂点
    float3 v3Start = v3CameraPos;
    //サンプルではカメラの位置が(0,Radius,0)なのでカメラの高さ。どの位置に移動しても地球視点で見れば原点(地球の中心)からの高さ。
    float fCameraHeight = length(v3CameraPos);
    //地上からの法線(?)と拡散光がやってきた角度の内積によって求められた角度をカメラの高さで割る。
    float fStartAngle = dot(v3Ray, v3Start) / fCameraHeight;
    //開始地点の高さに平均大気密度をかけた値の指数を求める？
    float fStartDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fCameraHeight));
    //開始地点のなにかの角度のオフセット。
    float fStartOffset = fStartDepth * Scale(fStartAngle);

    //サンプルポイント間の長さ。
    float fSampleLength = fFar / fSamples;
    //サンプルポイント間の長さに地球の大気の割合をかける。
    float fScaledLength = fSampleLength * fScale;
    //拡散光が来た方向にサンプルの長さをかけることでサンプルポイント間のレイをベクトルを求める。
    float3 v3SampleRay = v3Ray * fSampleLength;
    //最初のサンプルポイントを求める。0.5をかけてるのは少し動かすため？
    float3 v3SamplePoint = v3Start + v3SampleRay * 0.5f;

    //色情報
    float3 v3FrontColor = 0.0f;
    for (int n = 0; n < int(fSamples); ++n)
    {
        //サンプルポイントの高さ。どちらにせよ原点は地球の中心なので、この値が現在位置の高さになる。
        float fHeight = length(v3SamplePoint);
        //地上からサンプルポイントの高さの差に平均大気密度をかけたもの。  高度に応じて大気密度が指数的に小さくなっていくのを表現している？
        float fDepth = exp(fScaleOverScaleDepth * (fInnerRadius - fHeight));
        //地上から見たサンプルポイントの法線とディレクショナルライトの方向の角度を求めて、サンプルポイントの高さで割る。
        float fLightAngle = dot(v3LightDir, v3SamplePoint) / fHeight; //こいつの値が-1になる→Scale内の計算でexpの引数が43になり、とてつもなくでかい値が入る。 → -にならないようにする？
        //地上から見たサンプルポイントの法線と散乱光が飛んできている方区の角度を求めて、サンプルポイントの高さで割る。
        float fCameraAngle = dot(v3Ray, v3SamplePoint) / fHeight;
        //散乱光？
        float fScatter = (fStartOffset + fDepth * (Scale(fLightAngle * 1) - Scale(fCameraAngle * 1)));

        //色ごとの減衰率？
        float3 v3Attenuate = exp(-fScatter * (v3InvWaveLength * fKr4PI + fKm4PI));
        //サンプルポイントの位置を考慮して散乱した色を求める。
        v3FrontColor += v3Attenuate * (fDepth * fScaledLength);
        //サンプルポイントを移動させる。
        v3SamplePoint += v3SampleRay;

    }

    //レイリー散乱に使用する色情報
    float3 c0 = v3FrontColor * (v3InvWaveLength * fKrESun);
    //ミー散乱に使用する色情報
    float3 c1 = v3FrontColor * fKmESun;
    //カメラ座標から天球の座標へのベクトル。
    float3 v3Direction = v3CameraPos - worldPos;

    //float fcos = dot(v3LightDir, v3Direction) / length(v3Direction);
    float fcos = dot(v3LightDir, v3Direction) / length(v3Direction);
    float fcos2 = fcos * fcos;

    //レイリー散乱の明るさ。
    float rayleighPhase = 0.75f * (1.0f + fcos2);
    //ミー散乱の明るさ。
    float miePhase = 1.5f * ((1.0f - g2) / (2.0f + g2)) * (1.0f + fcos2) / pow(1.0f + g2 - 2.0f * g * fcos, 1.5f);

    //ミー散乱の色を保存。
    mieColor = c0 * rayleighPhase;

    //最終結果の色
    float3 col = 1.0f;
    col.rgb = rayleighPhase * c0 + miePhase * c1;

    //交点までのベクトルと太陽までのベクトルが近かったら白色に描画する。
    int sunWhite = step(0.999f, dot(normalize(dirLightPos - v3CameraPos), normalize(worldPos - v3CameraPos)));
    
    return col + float3(sunWhite, sunWhite, sunWhite);

}

//波を計算。
float SeaOctave(float2 arg_uv, float arg_choppy, float3 arg_position)
{
    
    float noise = ValueNoise(arg_uv, 0.0f) * 2.0f;
    arg_uv += float2(noise, noise * 1.0f);
    float2 wv = 1.0f - abs(sin(arg_uv));
    float2 swv = abs(cos(arg_uv));
    wv = lerp(wv, swv, wv);
    return pow(1.0f - pow(wv.x * wv.y, 0.65f), arg_choppy);
}

//海のハイトマップの計算の際にレイマーチングしている位置のノイズを計算する。
float MappingHeightNoise(float3 arg_position, int arg_samplingCount)
{
    
    //定数 いずれ定数バッファにする。
    float freq = debugSeaData.m_freq;
    float amp = debugSeaData.m_amp;
    float choppy = debugSeaData.m_choppy;
    float seaSpeed = debugSeaData.m_seaSpeed;
    
    float thickness = 8.0f;
    for (int index = 0; index < 8; ++index)
    {
        float waveLength = length(arg_position - shockWaveData.m_shockWave[index].m_pos) - shockWaveData.m_shockWave[index].m_radius;
        if (abs(waveLength) <= thickness)
        {
    
            float easing = (1.0f - abs(waveLength / thickness));
            //easing = easing ==  0 ? 0 : pow(2, 10 * easing - 10);
            easing = -(cos(PI * easing) - 1.0f) / 2.0f;
            amp += shockWaveData.m_shockWave[index].m_power * easing;
        
        }
    }
    

    //XZ平面による計算
    float2 uv = arg_position.xz / 2.0f;

    float d, h = 0.0f;
    
    //ここで「フラクタルブラウン運動」によるノイズの計算を行っている
    for (int i = 0; i < arg_samplingCount; ++i)
    {
        
        float seaTimer = (1.0f + cameraEyePos.m_timer * seaSpeed);
        
        //単純に、iTime（時間経過）によってUV値を微妙にずらしてアニメーションさせている
        //iTimeのみを指定してもほぼ同じアニメーションになる
        //SEA_TIMEのプラス分とマイナス分を足して振幅を大きくしている・・・？
        d = SeaOctave((uv + seaTimer) * freq, choppy, arg_position);
        d += SeaOctave((uv - seaTimer) * freq, choppy, arg_position);

        h += d * amp;

        float octave_m = float2x2(1.6f, 1.2f, -1.2f, 1.6f);
        //これは回転行列・・・？
        //uv値を回転させている風。これをなくすと平坦な海になる
        uv *= octave_m;
        //uv = mul(uv, octave_m);

        //fbm関数として、振幅を0.2倍、周波数を2.0倍して次の計算を行う
        freq *= 2.0f;
        amp *= 0.2f;

        //choppyを翻訳すると「波瀾」という意味
        //これを小さくすると海が「おとなしく」なる
        choppy = lerp(choppy, 1.0f, 0.2f);
    }

    //最後に、求まった高さ`h`を、現在のレイの高さから引いたものを「波の高さ」としている
    return arg_position.y - h;
}

//水面に向かってレイマーチングを行う。
void HeightMapRayMarching(float3 arg_origin, float3 arg_direction, out float3 arg_position)
{
    float tm = 0.0f;

    float tx = 10000.0f;

    //一旦遠くの位置のサンプリングを行い、結果の高さが0以上だったらレイが海に当たらないということなのでReturnする。
    float hx = MappingHeightNoise(arg_origin + arg_direction * tx, 8);
    if (0.0f < hx)
    {
        arg_position = float3(0.0f, 0.0f, 0.0f);
        return;
    }

    //ここからが本格的なレイマーチング。
    float hm = MappingHeightNoise(arg_origin + arg_direction * tm, 8); //開始地点でのハイトマップの値。
    float tmid = 0.0f;
    for (int i = 0; i < 8; ++i)
    {
        //現在の位置でのハイトマップの値をレイマーチングの到達点の値で正規化する。
        float f = hm / (hm - hx);

        tmid = lerp(tm, tx, f);
        arg_position = arg_origin + arg_direction * tmid;

        //次なる位置のハイトマップを取得する。
        float hmid = MappingHeightNoise(arg_position, 8);

        //サンプリング位置の高さがマイナス距離の場合は`hx`, `tx`を更新する
        if (hmid < 0.0f)
        {
            //遠くの位置
            tx = tmid;
            hx = hmid;
        }
        //そうでない場合は、`hm`, `tm`を更新する
        else
        {
            //近くの位置
            tm = tmid;
            hm = hmid;
        }
    }
    
}

float3 GetSeaNormal(float3 arg_position, float arg_eps)
{
    float3 n;
    n.y = MappingHeightNoise(arg_position, 5);
    n.x = MappingHeightNoise(float3(arg_position.x + arg_eps, arg_position.y, arg_position.z), 5) - n.y;
    n.z = MappingHeightNoise(float3(arg_position.x, arg_position.y, arg_position.z + arg_eps), 5) - n.y;
    n.y = arg_eps;
    return normalize(n);
}