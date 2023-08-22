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
void CastRay(inout Payload arg_payload, float3 arg_origin, float3 arg_dir, float arg_far, int arg_msIndex, RAY_FLAG arg_rayFlag, RaytracingAccelerationStructure arg_scene, uint arg_mask)
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
        arg_mask,
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
        
        //レイを撃つ
        bool isDebug = arg_debugRaytracingData.m_debugShadow == 1 && arg_launchIndex.x < arg_debugRaytracingData.m_sliderRate;
        if (isDebug || arg_isFar)
        {
            payloadData.m_color = float3(1, 1, 1);
        }
        else
        {
            CastRay(payloadData, arg_worldPosMap.xyz, -arg_lightData.m_dirLight.m_dir, 100.0f, MISS_LIGHTING, RAY_FLAG_SKIP_CLOSEST_HIT_SHADER, arg_scene, 0x01);
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

void GodRayPass(float4 arg_worldColor, inout float4 arg_albedoColor, uint2 arg_launchIndex, CameraEyePosConstData arg_cameraEyePos, LightData arg_lightData, RaytracingAccelerationStructure arg_scene, RWTexture3D<float4> arg_volumeTexture, RaymarchingParam arg_raymarchingParam)
{
    
    //カメラからサンプリング地点までをレイマーチングで動かす定数で割り、サンプリング回数を求める。
    const int SAMPLING_COUNT = 16;
    float3 samplingDir = normalize(arg_worldColor.xyz - arg_cameraEyePos.m_eye);
    float samplingLength = length(arg_cameraEyePos.m_eye - arg_worldColor.xyz) / (float) SAMPLING_COUNT;
    float raymarchingBright = 0.0f;
    bool isFinshVolumeFog = false;
    for (int counter = 0; counter < SAMPLING_COUNT; ++counter)
    {
                
        //ペイロード(再帰的に処理をするレイトレの中で値の受け渡しに使用する構造体)を宣言。
        Payload payloadData;
        payloadData.m_emissive = float3(0.0f, 0.0f, 0.0f);
        payloadData.m_color = float3(0.0f, 0.0f, 0.0f); //色を真っ黒にしておく。レイを飛ばしてどこにもあたらなかった時に呼ばれるMissShaderが呼ばれたらそこで1を書きこむ。何かに当たったときに呼ばれるClosestHitShaderが呼ばれたらそこは影なので0を書き込む。
        
        //減衰
        float progress = 1.0f / float(counter + 1.0f);
        
        //ディレクションライトの処理
        if (arg_lightData.m_dirLight.m_isActive)
        {
        
            //レイを撃つ
            CastRay(payloadData, arg_cameraEyePos.m_eye + (samplingDir * samplingLength) * counter, -arg_lightData.m_dirLight.m_dir, 300000.0f, MISS_LIGHTING, RAY_FLAG_SKIP_CLOSEST_HIT_SHADER, arg_scene, 0x01);
        
            //結果を保存。
            raymarchingBright = lerp(raymarchingBright, payloadData.m_color.x, progress);
            
        }
        //ポイントライトの処理
        float pointLightDistance = length((arg_cameraEyePos.m_eye + (samplingDir * samplingLength) * counter) - arg_worldColor.xyz);
        if (arg_lightData.m_pointLight.m_isActive && pointLightDistance < arg_lightData.m_pointLight.m_power)
        {
        
            //レイを撃つ
            CastRay(payloadData, arg_cameraEyePos.m_eye + (samplingDir * samplingLength) * counter, -arg_lightData.m_dirLight.m_dir, pointLightDistance, MISS_LIGHTING, RAY_FLAG_SKIP_CLOSEST_HIT_SHADER, arg_scene, 0x01);
        
            //結果を保存。
            raymarchingBright = lerp(raymarchingBright, payloadData.m_color.x, progress);
            
        }
        
    }
    
    //ボリュームフォグ
    float3 fogColor = float3(0, 0, 0);
    if (arg_raymarchingParam.m_isActive)
    {
        //レイマーチングの回数を計算。
        float rayLength = length(arg_cameraEyePos.m_eye - arg_worldColor.xyz);
        float marchingMovedLength = 0; //レイマーチングで動いた距離
        float3 marchingPos = arg_cameraEyePos.m_eye;
        float3 marchingDir = normalize(arg_worldColor.xyz - arg_cameraEyePos.m_eye);
        for (int index = 0; index < 10000; ++index)
        {
        
            //マーチングを進める量。
            float marchingMoveLength = arg_raymarchingParam.m_sampleLength;
        
            //マーチングが上限より移動していたら。
            bool isFinish = false;
            if (rayLength < marchingMovedLength + marchingMoveLength)
            {
            
                //残りの量を移動させる。
                marchingMoveLength = rayLength - marchingMovedLength;
                isFinish = true;

            }
            else
            {
            
                //動いた量を保存。
                marchingMovedLength += marchingMoveLength;
            
            }
        
            //マーチングを進める。
            marchingPos += marchingDir * marchingMoveLength;
        
            //レイマーチングの座標をボクセル座標空間に直す。
            float3 volumeTexPos = arg_raymarchingParam.m_pos;
            volumeTexPos -= arg_raymarchingParam.m_gridSize * ((256.0f / 2.0f) * arg_raymarchingParam.m_wrapCount);
            int3 boxPos = marchingPos - volumeTexPos; //マーチングのサンプリング地点をボリュームテクスチャの中心基準の座標にずらす。
            boxPos /= arg_raymarchingParam.m_gridSize;
        
            //マーチング座標がボクセルの位置より離れていたらサンプリングしない。
            if (!IsInRange(boxPos, 256.0f, arg_raymarchingParam.m_wrapCount))
            {
        
                if (isFinish)
                {
                    break;
                }
                continue;
            }
        
            boxPos.x = boxPos.x % 256;
            boxPos.y = boxPos.y % 256;
            boxPos.z = boxPos.z % 256;
            boxPos = clamp(boxPos, 0, 255);
        
            //ノイズを抜き取る。
            float3 noise = arg_volumeTexture[boxPos].xyz / 50.0f;
        
            float3 weights = float3(0.8f, 0.1f, 0.1f); // 各ノイズの重み
            float fogDensity = dot(noise, weights) * arg_raymarchingParam.m_density;
        
            //Y軸の高さで減衰させる。
            float maxY = 50.0f;
           // fogDensity *= 1.0f - saturate(marchingPos.y / maxY);
        
            //その部分の色を抜き取る。
            fogColor += float3(fogDensity, fogDensity, fogDensity) * arg_raymarchingParam.m_color;
            //fogColor = arg_raymarchingParam.m_color * fogDensity + fogColor * saturate(1.0f - fogDensity);
        
            if (isFinish)
            {
                break;
            }
        
        }
    }
    
    const float3 FOGCOLOR_LIT = float3(1.0f, 1.0f, 1.0f);
    const float3 FOGCOLOR_UNLIT = float3(0.0f, 0.0f, 0.0f);
    
    float3 godRayColor = lerp(FOGCOLOR_UNLIT, FOGCOLOR_LIT, raymarchingBright);
    const float FOG_DENSITY = 0.0001f;
    float absorb = exp(-length(arg_cameraEyePos.m_eye - arg_worldColor.xyz) * FOG_DENSITY);
    arg_albedoColor.xyz = lerp(godRayColor, arg_albedoColor.xyz, absorb);
    arg_albedoColor.xyz += float3(clamp(fogColor.x, 0.0f, arg_raymarchingParam.m_color.x), clamp(fogColor.y, 0.0f, arg_raymarchingParam.m_color.y), clamp(fogColor.z, 0.0f, arg_raymarchingParam.m_color.z));
    
}

void SecondaryPass(float3 arg_viewDir, inout float4 arg_emissiveColor, float4 arg_worldColor, float4 arg_materialInfo, float4 arg_normalColor, float4 arg_albedoColor, RaytracingAccelerationStructure arg_scene, CameraEyePosConstData arg_cameraEyePos, inout float4 arg_finalColor)
{
        
    //レイのIDをみて、レイを打つかどうかを判断
    if (arg_materialInfo.w == MATERIAL_REFRACT && 0.1f < length(arg_normalColor.xyz))
    {
        
        Payload payloadData;
        payloadData.m_emissive = float3(0.0f, 0.0f, 0.0f);
        payloadData.m_color = float3(1, 1, 1);
        
        //レイを撃つ
        float3 rayOrigin = arg_worldColor.xyz + arg_normalColor.xyz * 3.0f;
        CastRay(payloadData, rayOrigin, refract(arg_viewDir, arg_normalColor.xyz, 0.1f), 100.0f, MISS_DEFAULT, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, arg_scene, 0x01);
        
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
        
        //レイを撃つ
        float3 rayOrigin = arg_worldColor.xyz + arg_normalColor.xyz * 3.0f;
        CastRay(payloadData, rayOrigin, reflect(arg_viewDir, arg_normalColor.xyz), 100.0f, MISS_DEFAULT, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, arg_scene, 0x01);
        
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
        Payload reflectionColor;
        reflectionColor.m_emissive = float3(0.0f, 0.0f, 0.0f);
        reflectionColor.m_color = float3(1, 1, 1);
        
        //レイを撃つ
        float3 rayOrigin = arg_worldColor.xyz;
        CastRay(refractionColor, rayOrigin, refract(arg_viewDir, arg_normalColor.xyz, 0.1f), 500.0f, MISS_CHECKHIT, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, arg_scene, 0x01);
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
        
        arg_emissiveColor.xyz += refractionColor.m_emissive;
        arg_emissiveColor.xyz += reflectionColor.m_emissive;
        
    }
    else
    {
        arg_finalColor = arg_albedoColor;
    }
}