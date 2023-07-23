Texture2D<float4> luminanceMap : register(t0);
RWTexture2D<float4> extractedLuminanceMap : register(u0);

[numthreads(1280, 720, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    
    float2 texSize = float2(1280.0f, 720.0f);
    
    //extractedLuminanceMap[DTid.xy] = max(float4(0.0f, 0.0f, 0.0f, 0.0f), luminanceMap.SampleLevel(uInputTex, vTexcoord) + uBias) * uScale;
}