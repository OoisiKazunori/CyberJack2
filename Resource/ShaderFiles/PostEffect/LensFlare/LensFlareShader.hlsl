
//入力情報
Texture2D<float4> LensColor : register(t0);

//入力情報
RWTexture2D<float4> InputImg : register(u0);

//出力先UAV  
RWTexture2D<float4> OutputImg : register(u1);

//色収差
float3 TextureDistorted(float2 texcoord, float2 direction, uint2 texSize, float3 distortion)
{
    return float3(
         InputImg[(texcoord + direction * distortion.r) * texSize].r,
         InputImg[(texcoord + direction * distortion.g) * texSize].g,
         InputImg[(texcoord + direction * distortion.b) * texSize].b
      );
}

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{

    //テクスチャのサイズ
    const float2 TEXSIZE = float2(1280.0f, 720.0f);
    const float2 LENSCOLOR_TEXSIZE = float2(256.0f, 1.0f);
    const float2 WINDOW_CENTER = float2(0.5f, 0.5f);
    
    //サンプリングするテクスチャの上下左右を反転させる。
    float2 texpos = float2(DTid.x, DTid.y) / TEXSIZE;
    texpos = -texpos + float2(1.0f, 1.0f);
 
    //画面中心に向かってのベクトルを求める。ゴーストにを飛ばす方向。
    float ghostDispersal = 0.36f;
    float2 ghostVec = (WINDOW_CENTER - texpos) * ghostDispersal;
    
    //色収差用パラメーターを計算。
    float distortionValue = 15.0f;
    float2 texelSize = 1.0f / TEXSIZE;
    float3 distortion = float3(-texelSize.x * distortionValue, 0.0f, texelSize.x * distortionValue);
    float2 direction = normalize(ghostVec);
   
    //ゴーストをサンプリングする。
    float4 sampleResult = float4(0,0,0,1);
    uint ghostCount = 8;
    for (int i = 0; i < ghostCount; ++i)
    {
        float2 offset = frac(texpos + ghostVec * float(i));
        
        float weight = length(WINDOW_CENTER - offset) / length(WINDOW_CENTER);
        weight = pow(1.0f - weight, 10.0f);
  
        //sampleResult.xyz += InputImg[(offset) * TEXSIZE] * weight;
        sampleResult.xyz += TextureDistorted((offset), direction, TEXSIZE, distortion) * weight;

    }
    
    //レンズの色を適用。
    uint2 lensColorUV = (length(WINDOW_CENTER - texpos) / length(WINDOW_CENTER)) * LENSCOLOR_TEXSIZE;
    sampleResult *= LensColor[lensColorUV];
    
    //ハローをサンプリング
    float haloWidth = 0.47f;
    float2 haloVec = normalize(ghostVec) * haloWidth;
    float weight = length(WINDOW_CENTER - frac(texpos + haloVec)) / length(WINDOW_CENTER);
    weight = pow(1.0f - weight, 5.0f);
    //sampleResult.xyz += InputImg[(texpos + haloVec) * TEXSIZE] * weight;
    sampleResult.xyz += TextureDistorted(texpos + haloVec, direction, TEXSIZE, distortion) * weight;
    
    OutputImg[DTid.xy] = sampleResult;
    
}