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
ConstantBuffer<DebugRaytracingParam> debugRaytracingData : register(b3);

//GBuffer
Texture2D<float4> albedoMap : register(t1);
Texture2D<float4> normalMap : register(t2);
Texture2D<float4> materialMap : register(t3);
Texture2D<float4> worldMap : register(t4);

//出力先UAV
RWTexture2D<float4> finalColor : register(u0);
RWTexture3D<float4> volumeNoiseTexture : register(u1);
RWTexture2D<float4> lensFlareTexture : register(u2);

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
    float2 uv = arg_position.xz / 2.0f;

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
    
    //遠さを見る。
    const float REFLECTION_DEADLINE = 10000.0f;
    bool isFar = REFLECTION_DEADLINE < length(cameraEyePos.m_eye - worldColor.xyz);
    
    //海を描画
    bool isSea = 0 < cameraEyePos.m_eye.y && (cameraEyePos.m_eye + dir * 10000.0f).y < 0 && length(normalColor.xyz) < 0.1f;
    isSea |= cameraEyePos.m_eye.y < 0 && 0 < (cameraEyePos.m_eye + dir * 10000.0f).y && length(normalColor.xyz) < 0.1f;
    isSea |= cameraEyePos.m_eye.y < 0 && 0 < worldColor.y;
    isSea |= 0 < cameraEyePos.m_eye.y && worldColor.y < 0;
    if (isSea)
    {
        
        float3 origin = cameraEyePos.m_eye;
        
        //Y-だったらY軸を反転
        if (origin.y < 0)
        {
            origin.y *= -1.0f;
            dir.y *= -1.0f;
        }
        
        float3 position;
        HeightMapRayMarching(origin, dir, position);

        float3 dist = position - origin;
        float3 n = GetNormal(position, dot(dist, dist) * (0.1f / dims.x));
        
        float3 mieColor = float3(0, 0, 0);
        float3 sky = float3(0,0,0);
        bool isDebug = debugRaytracingData.m_debugReflection == 1 && launchIndex.x < debugRaytracingData.m_sliderRate;
        isFar = REFLECTION_DEADLINE < length(cameraEyePos.m_eye - position);
        if (isDebug)
        {
            sky = GetSkyColor(dir);
        }
        else
        {
            sky = AtmosphericScattering(reflect(dir, n) * 15000.0f, mieColor);
        }
        float3 sea = GetSeaColor(position, n, lightData.m_dirLight.m_dir, dir, dist);
        
        float t = pow(smoothstep(0.0f, -0.05f, dir.y), 0.3f);
        //float3 color = lerp(sky, sea, t);
        float3 color = float3(0,0,0);
        
        albedoColor.xyz = color;
        worldColor.xyz = position;
        normalColor.xyz = n;
        materialInfo.y = 0.8f;
        materialInfo.w = MATERIAL_SEA;

    }
    
    //ライティングパスを行う。
    float bright = 0.0f;
    LightingPass(bright, worldColor, normalColor, lightData, launchIndex, debugRaytracingData, gRtScene, isFar);
    
    //輝度が一定以上だったらレンズフレア用のテクスチャに書きこむ。
    const float LENSFLARE_DEADLINE = 0.3f;
    float deadline = step(LENSFLARE_DEADLINE, bright);
    float lensflareBright = (deadline * bright);
    lensFlareTexture[launchIndex.xy] = float4(albedoColor.xyz * lensflareBright * 0.1f, 1.0f);
    
    //アルベドにライトの色をかける。
    albedoColor.xyz *= clamp(bright, 0.3f, 1.0f);
    
    //GodRayPass
    //GodRayPass(worldColor, albedoColor, launchIndex, cameraEyePos, lightData, gRtScene, volumeNoiseTexture, volumeFogData);
    
    //マテリアルのIDをもとに、反射屈折のレイを飛ばす。
    float4 final = float4(0, 0, 0, 1);
    bool isDebug = debugRaytracingData.m_debugReflection == 1 && launchIndex.x < debugRaytracingData.m_sliderRate;
    if (isDebug || isFar)
    {
        final = albedoColor;
    }
    else
    {
        SecondaryPass(dir, worldColor, materialInfo, normalColor, albedoColor, gRtScene, cameraEyePos, final);
    }
    
    //描画されているけど水中！だったら水中っぽい見た目にする。
    if (cameraEyePos.m_eye.y < 0 && worldColor.y < 0)
    {
        float perOfSea = 0.5f;
        final = float4(albedoColor.xyz, 1) * perOfSea;
        final += float4(SEA_BASE, 1) * (1.0f - perOfSea);
    }
    //なにも描画されていないところでは空の色を取得。
    else if (length(worldColor.xyz) < 0.1f && length(normalColor.xyz) < 0.1f && !isSea)
    {
        
        //final.xyz = GetSkyColor(dir);
        float3 mieColor = float3(0, 0, 0);
        final.xyz = AtmosphericScattering(dir * 15000.0f, mieColor);
        lensFlareTexture[launchIndex.xy].xyz += mieColor * 0.1f;
        
        //下方向を向いていたら海を描画
        if ((cameraEyePos.m_eye + dir * 1000).y < 0)
        {
            final.xyz = SEA_BASE;
        }
        
    }
    
    //合成の結果を入れる。
    finalColor[launchIndex.xy] = final;
  
}

//missシェーダー レイがヒットしなかった時に呼ばれるシェーダー
[shader("miss")]
void mainMS(inout Payload PayloadData)
{
 
    float3 mieColor = float3(0, 0, 0);
    PayloadData.m_color = AtmosphericScattering(WorldRayDirection() * 15000.0f, mieColor);

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