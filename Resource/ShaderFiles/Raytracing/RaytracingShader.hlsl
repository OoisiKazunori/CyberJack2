#include "RaytracingShaderHeader.hlsli"

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
    float4 emissiveColor = emissiveMap[launchIndex];
    
    //法線が-1,-1,-1だったらパーティクルなので処理を飛ばす。
    if (normalColor.r <= -0.9f && normalColor.g <= -0.9f && normalColor.b <= -0.9f)
    {

        finalColor[launchIndex.xy] = albedoColor;
        emissiveTexture[launchIndex.xy] = emissiveColor;
        return;
        
    }
    
    //遠さを見る。 ある程度離れている位置では反射の計算を切る
    const float REFLECTION_DEADLINE = 10000.0f;
    bool isFar = REFLECTION_DEADLINE < length(cameraEyePos.m_eye - worldColor.xyz);
    
    //海を描画
    bool isSea = 0 < cameraEyePos.m_eye.y && (cameraEyePos.m_eye + dir * 10000.0f).y < 0 && length(normalColor.xyz) < 0.1f;
    isSea |= 0 < cameraEyePos.m_eye.y && worldColor.y < 0;
    if (isSea)
    {
        
        //視点を設定してレイマーチングを行う。
        float3 origin = cameraEyePos.m_eye;
        float3 position;
        HeightMapRayMarching(origin, dir, position);

        //距離から海の法線を取得する。
        float3 dist = position - origin;
        float3 n = GetSeaNormal(position, dot(dist, dist) * (0.1f / dims.x));
        
        
        //海に反射している空の色を計算する。
        float3 mieColor = float3(0, 0, 0);
        float3 sky = float3(0, 0, 0);
        bool isDebug = debugRaytracingData.m_debugReflection == 1 && launchIndex.x < debugRaytracingData.m_sliderRate;
        isFar = REFLECTION_DEADLINE < length(cameraEyePos.m_eye - position);
        //レイトレーシングの反射を切るデバッグがOnになっていれば、定数の空のいろを取得する。
        if (isDebug)
        {
            sky = GetSkyColor(normalize(reflect(dir, n)));
        }
        else
        {
            float3 sampleVec = normalize(reflect(dir, n));
            //ベクトルが下方向を向いている場合、海の色にする。
            if (sampleVec.y < 0.0f)
            {
                sky = GetSeaColor(position, n, lightData.m_dirLight.m_dir, dir, dist);
            }
            else
            {
                sky = AtmosphericScattering(sampleVec * 15000.0f, mieColor);
            }
        }
        
        //海の色を求める。
        float3 sea = GetSeaColor(position, n, lightData.m_dirLight.m_dir, dir, dist);
        
        float t = pow(smoothstep(0.0f, -0.05f, dir.y), 0.3f);
        float3 color = lerp(sky * debugRaytracingData.m_skyFacter, sea, t);
        
        albedoColor.xyz = color;
        worldColor.xyz = position;
        normalColor.xyz = n;
        materialInfo.y = 0.8f;
        materialInfo.w = MATERIAL_SEA;
        emissiveColor = float4(0, 0, 0, 0);

    }
    
    //ライティングパスを行う。
    float bright = 0.0f;
    //発光しているオブジェクトは発光具合を明るさにする。
    if (0 < length(emissiveColor.xyz))
    {
        bright += length(emissiveColor.xyz);
    }
    //反射屈折するオブジェクトのライティングは切る。
    else if (MATERIAL_REFLECT == materialInfo.y || MATERIAL_REFRACT == materialInfo.y)
    {
        bright = 1.0f;
    }
    //ライティングを行う。
    else
    {
        LightingPass(bright, worldColor, normalColor, lightData, launchIndex, debugRaytracingData, gRtScene, isFar);
        
        
    }
    
    //輝度が一定以上だったらレンズフレア用のテクスチャに書きこむ。
    const float LENSFLARE_DEADLINE = 0.3f;
    float deadline = step(LENSFLARE_DEADLINE, bright);
    float lensflareBright = (deadline * bright);
    lensFlareTexture[launchIndex.xy] = saturate(float4(albedoColor.xyz * lensflareBright * 0.1f, 1.0f) + emissiveColor * 0.45f);
    
    //アルベドにライトの色をかける。
    albedoColor.xyz *= clamp(bright, 0.3f, 1.0f);
    
    //マテリアルのIDをもとに、反射屈折のレイを飛ばす。
    float4 final = float4(0, 0, 0, 1);
    bool isDebug = debugRaytracingData.m_debugReflection == 1 && launchIndex.x < debugRaytracingData.m_sliderRate;
    if (isDebug || isFar)
    {
        final = albedoColor;
    }
    else
    {
        SecondaryPass(dir, emissiveColor, worldColor, materialInfo, normalColor, albedoColor, gRtScene, cameraEyePos, final);

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
        final.xyz = AtmosphericScattering(dir * 15000.0f, mieColor) * debugRaytracingData.m_skyFacter;
        lensFlareTexture[launchIndex.xy].xyz += mieColor * 0.1f;
        
        //下方向を向いていたら海を描画
        if ((cameraEyePos.m_eye + dir * 1000).y < 0)
        {
            final.xyz = SEA_BASE;
        }
        
    }
    
    //合成の結果を入れる。
    finalColor[launchIndex.xy] = final;
    emissiveTexture[launchIndex.xy] = emissiveColor;
  
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
    
    
    //rayidが1なら影用のライト
    if (payload.m_rayID == 1)
    {
        
        uint instanceID = InstanceID();
    
        int octave = 6;
        float persitance = 2.0f;
        float lacunarity = 1.25f;
    
        float noise = PerlinNoise2D(vtx.uv, octave, persitance, lacunarity, shockWaveData.m_shockWave[clamp(instanceID - 2, 0, 10)].m_facter);
    
        if (noise <= 0.01f)
        {
            payload.m_color = float3(1, 0, 0);
        }
        else
        {
            payload.m_color = float3(0, 0, 0);
        }
        
    }
    //rayidが2なら敵の反射屈折
    else if (payload.m_rayID == 2)
    {
        
        //反射先のライティングを行う。
        float bright = 0;
        const float REFLECTION_DEADLINE = 10000.0f;
        bool isFar = REFLECTION_DEADLINE < length(cameraEyePos.m_eye - vtx.pos.xyz);
        LightingPass(bright, float4(WorldRayOrigin(), 1.0f), float4(vtx.normal, 1.0f), lightData, DispatchRaysIndex(), debugRaytracingData, gRtScene, isFar);
        payload.m_color *= clamp(bright, 0.3f, 1.0f);
    
        int octave = 6;
        float persitance = 2.0f;
        float lacunarity = 1.25f;
    
        uint instanceID = InstanceID();
        float noise = PerlinNoise2D(vtx.uv, octave, persitance, lacunarity, shockWaveData.m_shockWave[clamp(instanceID - 2, 0, 10)].m_facter);
        
        if (noise <= 0.01f)
        {
            payload.m_color = float3(-1, -1, -1);
        }
        
        
    
        //当たったオブジェクトのInstanceIDが1だったら(GPUパーティクルだったら)輝度を保存する。
        if (instanceID == 1)
        {
            payload.m_emissive = payload.m_color;
        }
        else
        {
            payload.m_emissive = payload.m_color / 3.0f;

        }
        
    }
    //それ以外は通常の反射
    else
    {
    
        //当たったオブジェクトのInstanceIDが1だったら(GPUパーティクルだったら)輝度を保存する。
        uint instanceID = InstanceID();
        if (instanceID == 1)
        {
            payload.m_emissive = payload.m_color;
        }
        else
        {
            //反射先のライティングを行う。
            float bright = 0;
            const float REFLECTION_DEADLINE = 10000.0f;
            bool isFar = REFLECTION_DEADLINE < length(cameraEyePos.m_eye - vtx.pos.xyz);
            LightingPass(bright, float4(WorldRayOrigin(), 1.0f), float4(vtx.normal, 1.0f), lightData, DispatchRaysIndex(), debugRaytracingData, gRtScene, isFar);
            payload.m_color *= clamp(bright, 0.3f, 1.0f);

        }
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