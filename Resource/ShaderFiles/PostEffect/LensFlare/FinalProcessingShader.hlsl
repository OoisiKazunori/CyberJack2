
Texture2D<float4> LensDirt : register(t0);
Texture2D<float4> LensStar : register(t1);

//入力情報
RWTexture2D<float4> InputImg : register(u0);
RWTexture2D<float4> BloomImg : register(u1);

//出力先UAV  
RWTexture2D<float4> OutputImg : register(u2);

cbuffer CameraVec : register(b0)
{
    float3 m_cameraX;
    float m_pad;
    float3 m_cameraZ;
    float m_pad2;
};

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    const float2 TEXSIZE = float2(1280.0f, 720.0f);
    const float2 LENSDIRT_TEXSIZE = float2(1920.0f, 1080.0f);
    const float2 LENSSTAR_TEXSIZE = float2(1024.0f, 1024.0f);
    
    //レンズの汚れをサンプリング
    float4 lensMod = LensDirt[float2(DTid.xy / TEXSIZE) * LENSDIRT_TEXSIZE] / 0.2f; //画像が暗すぎるのでちょっとだけ値を大きくする。
    
    //スターバーストをサンプリング
    float camrot = dot(m_cameraX, float3(0, 0, 1)) + dot(m_cameraZ, float3(0, 1, 0));
    float3x3 scaleBias1 = (
      2.0f, 0.0f, -1.0f,
      0.0f, 2.0f, -1.0f,
      0.0f, 0.0f, 1.0f
    );
    float3x3 rotation = (
      cos(camrot), -sin(camrot), 0.0f,
      sin(camrot), cos(camrot), 0.0f,
      0.0f, 0.0f, 1.0f
   );
    float3x3 scaleBias2 = (
      0.5f, 0.0f, 0.5f,
      0.0f, 0.5f, 0.5f,
      0.0f, 0.0f, 1.0f
    );
    float3x3 uLensStarMatrix = mul(mul(scaleBias2, rotation), scaleBias1);
    float2 lensStarTexcoord = mul(float3(float2(DTid.xy / TEXSIZE), 1.0f), uLensStarMatrix).xy;
    //lensMod += saturate(LensStar[lensStarTexcoord * LENSSTAR_TEXSIZE]) / 0.7f;
    lensMod += saturate(LensStar[float2(DTid.xy / TEXSIZE) * LENSSTAR_TEXSIZE] / 0.7f);
    
    OutputImg[DTid.xy] = InputImg[DTid.xy] * lensMod + BloomImg[DTid.xy];
    
}