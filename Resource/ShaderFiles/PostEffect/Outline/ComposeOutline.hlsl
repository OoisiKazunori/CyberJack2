//���͏��
RWTexture2D<float4> Albedo : register(u0);
RWTexture2D<float4> Emissive : register(u1);

//�o�͐�UAV  
RWTexture2D<float4> OutlineAlbedo : register(u2);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    Albedo[DTid.xy] += OutlineAlbedo[DTid.xy];
    Emissive[DTid.xy] += OutlineAlbedo[DTid.xy];
}