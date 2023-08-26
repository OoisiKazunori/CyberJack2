
RWTexture2D<float4> Target : register(u0);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    Target[DTid.xy] = float4(0, 0, 0, 0);

}