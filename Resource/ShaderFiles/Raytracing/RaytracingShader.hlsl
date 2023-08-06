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
RWTexture2D<float4> lensFlareTexture : register(u2);

//波を計算。
float SeaOctave(float2 arg_uv, float arg_choppy)
{
    arg_uv += ValueNoise(arg_uv);
    float2 wv = 1.0f - abs(sin(arg_uv));
    float2 swv = abs(cos(arg_uv));
    wv = lerp(wv, swv, wv);
    return pow(1.0f - pow(wv.x * wv.y, 0.65f), arg_choppy);
}

//海のハイトマップの計算の際にレイマーチングしている位置のノイズを計算する。
float MappingHeightNoise(float3 arg_position)
{
    //定数 いずれ定数バッファにする。
    float freq = 0.16f;
    float amp = 0.6f;
    float choppy = 4.0f;
    float seaSpeed = 5.8f;

    //XZ平面による計算
    float2 uv = arg_position.xz;

    float d, h = 0.0f;
    
    //ここで「フラクタルブラウン運動」によるノイズの計算を行っている
    for (int i = 0; i < 4; ++i)
    {
        
        float seaTimer = (1.0f + cameraEyePos.m_timer * seaSpeed);
        
        //単純に、iTime（時間経過）によってUV値を微妙にずらしてアニメーションさせている
        //iTimeのみを指定してもほぼ同じアニメーションになる
        //SEA_TIMEのプラス分とマイナス分を足して振幅を大きくしている・・・？
        d = SeaOctave((uv + seaTimer) * freq, choppy);
        d += SeaOctave((uv - seaTimer) * freq, choppy);

        h += d * amp;

        float octave_m = float2x2(1.6f, 1.2f, -1.2f, 1.6f);
        //これは回転行列・・・？
        //uv値を回転させている風。これをなくすと平坦な海になる
        uv *= octave_m;

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
float HeightMapRayMarching(float3 arg_origin, float3 arg_direction, out float3 arg_position)
{
    float tm = 0.0f;

    float tx = 10000.0f;

    //一旦遠くの位置のサンプリングを行い、結果の高さが0以上だったらレイが海に当たらないということなのでReturnする。
    float hx = MappingHeightNoise(arg_origin + arg_direction * tx);
    if (0.0f < hx)
    {
        arg_position = float3(0.0f, 0.0f, 0.0f);
        return tx;
    }

    //ここからが本格的なレイマーチング。
    float hm = MappingHeightNoise(arg_origin + arg_direction * tm); //開始地点でのハイトマップの値。
    float tmid = 0.0f;
    for (int i = 0; i < 8; ++i)
    {
        //現在の位置でのハイトマップの値をレイマーチングの到達点の値で正規化する。
        float f = hm / (hm - hx);

        tmid = lerp(tm, tx, f);
        arg_position = arg_origin + arg_direction * tmid;

        //次なる位置のハイトマップを取得する。
        float hmid = MappingHeightNoise(arg_position);

        //サンプリング位置の高さがマイナス距離の場合は`hx`, `tx`を更新する
        if (hmid < 0.0f)
        {
            tx = tmid;
            hx = hmid;
        }
        //そうでない場合は、`hm`, `tm`を更新する
        else
        {
            tm = tmid;
            hm = hmid;
        }
    }
    
    return tmid;
}

float3 GetNormal(float3 arg_position, float arg_eps)
{
    float3 n;
    n.y = MappingHeightNoise(arg_position);
    n.x = MappingHeightNoise(float3(arg_position.x + arg_eps, arg_position.y, arg_position.z)) - n.y;
    n.z = MappingHeightNoise(float3(arg_position.x, arg_position.y, arg_position.z + arg_eps)) - n.y;
    n.y = arg_eps;
    return normalize(n);
}

//RayGenerationシェーダー
[shader("raygeneration")]
void mainRayGen()
{

    //現在のレイのインデックス。左上基準のスクリーン座標として使える。
    uint2 launchIndex = DispatchRaysIndex().xy;
    
    //カメラから見たレイの方向を計算。
    float2 dims = float2(DispatchRaysDimensions().xy);
    float2 d = (launchIndex.xy + 0.5f) / dims.xy * 2.0f - 1.0f;
    float aspect = dims.x / dims.y;
    float4 target = mul(cameraEyePos.m_projMat, float4(d.x, -d.y, 1, 1));
    float3 dir = mul(cameraEyePos.m_viewMat, float4(target.xyz, 0)).xyz;
    
    //GBufferから値を抜き取る。
    float4 albedoColor = albedoMap[launchIndex];
    float4 normalColor = normalMap[launchIndex];
    float4 materialInfo = materialMap[launchIndex];
    float4 worldColor = worldMap[launchIndex];
    
    //海を描画
    bool isSea = 0 < cameraEyePos.m_eye.y && (cameraEyePos.m_eye + dir * 10000.0f).y < 0 && length(normalColor.xyz) < 0.1f;
    isSea |= cameraEyePos.m_eye.y < 0 && 0 < (cameraEyePos.m_eye + dir * 10000.0f).y && length(normalColor.xyz) < 0.1f;
    isSea |= cameraEyePos.m_eye.y < 0 && 0 < worldColor.y;
    isSea |= 0 < cameraEyePos.m_eye.y && worldColor.y < 0;
    if (isSea)
    {
        float2 uv = launchIndex.xy / dims.xy;
        uv = uv * 2.0f - 1.0f;
        
        float3 origin = cameraEyePos.m_eye;
        
        //Y-だったらY軸を反転
        if (origin.y < 0)
        {
            origin.y *= -1.0f;
            //dir.y *= -1.0f;
        }
        
        float3 position;
        HeightMapRayMarching(origin, dir, position);

        float3 dist = position - origin;
        float3 n = GetNormal(position, dot(dist, dist) * (0.1f / dims.x));
        
        float3 sky = GetSkyColor(dir);
        float3 sea = GetSeaColor(position, n, lightData.m_dirLight.m_dir, dir, dist);
        
        float t = pow(smoothstep(0.0f, -0.05f, dir.y), 0.3f);
        float3 color = lerp(sky, sea, t);
        
        albedoColor.xyz = color;
        worldColor.xyz = position;
        normalColor.xyz = n;
        materialInfo.y = 0.8f;
        materialInfo.w = MATERIAL_SEA;

    }
    
    //ライティングパスを行う。
    float bright = 0.0f;
    LightingPass(bright, worldColor, normalColor, lightData, gRtScene);
    
    //輝度が一定以上だったらレンズフレア用のテクスチャに書きこむ。
    const float LENSFLARE_DEADLINE = 0.3f;
    float deadline = step(LENSFLARE_DEADLINE, bright);
    float lensflareBright = deadline * bright;
    lensFlareTexture[launchIndex.xy] = float4(albedoColor.xyz * lensflareBright, 1.0f);
    
    //アルベドにライトの色をかける。
    albedoColor.xyz *= clamp(bright, 0.3f, 1.0f);
    
    //GodRayPass
    GodRayPass(worldColor, albedoColor, launchIndex, cameraEyePos, lightData, gRtScene, volumeNoiseTexture, volumeFogData);
    
    //マテリアルのIDをもとに、反射屈折のレイを飛ばす。
    float4 final = float4(0, 0, 0, 1);
    SecondaryPass(dir, worldColor, materialInfo, normalColor, albedoColor, gRtScene, cameraEyePos, final);
    
    //なにも描画されていないところでは空の色を取得。
    if (length(worldColor.xyz) < 0.1f && length(normalColor.xyz) < 0.1f && !isSea)
    {
        
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

//当たり判定チェック用missシェーダー 
[shader("miss")]
void checkHitRayMS(inout Payload payload)
{
    
    payload.m_color = float3(-1, -1, -1);

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
    float4 mainTexColor = objectTexture.SampleLevel(smp, vtx.uv, 0);
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