
Texture2D<float4> LensDirt : register(t0);

//入力情報
RWTexture2D<float4> InputImg : register(u0);

//出力先UAV  
RWTexture2D<float4> OutputImg : register(u1);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    const float2 TEXSIZE = float2(1280.0f, 720.0f);
    const float2 LENSDIRT_TEXSIZE = float2(1920.0f, 1080.0f);
    
    //レンズの汚れをサンプリング
    float4 lensdirt = LensDirt[float2(DTid.xy / TEXSIZE) * LENSDIRT_TEXSIZE];
    
    OutputImg[DTid.xy] = InputImg[DTid.xy] * lensdirt;
    
}