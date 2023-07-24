
//入力情報
RWTexture2D<float4> InputImg : register(u0);

//出力先UAV  
RWTexture2D<float4> OutputImg : register(u1);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{

    //テクスチャのサイズ
    const float2 TEXSIZE = float2(1280.0f, 720.0f);
    
    //サンプリングするテクスチャの上下左右を反転させる。
    float2 texpos = float2(DTid.x, DTid.y) / TEXSIZE;
    texpos = -texpos + float2(1.0f, 1.0f);
    
    //いずれ使う。
    float2 texelSize = 1.0f / TEXSIZE;
 
    //画面中心に向かってのベクトルを求める。ゴーストにを飛ばす方向。
    float ghostDispersal = 0.36f;
    float2 ghostVec = (float2(0.5f, 0.5f) - texpos) * ghostDispersal;
   
    //ゴーストをサンプリングする。
    float4 sampleGhost = float4(0,0,0,0);
    uint ghostCount = 8;
    for (int i = 0; i < ghostCount; ++i)
    {
        float2 offset = frac(texpos + ghostVec * float(i));
        
        float weight = length(float2(0.5f, 0.5f) - offset) / length(float2(0.5f, 0.5f));
        weight = pow(1.0 - weight, 10.0);
  
        sampleGhost += InputImg[uint2(offset * TEXSIZE)] * weight;
    }
    
    OutputImg[DTid.xy] = sampleGhost;
    
}