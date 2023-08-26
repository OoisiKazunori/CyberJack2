//“ü—Íî•ñ
RWTexture2D<float4> Albedo : register(u0);
RWTexture2D<float4> Emissive : register(u1);

//o—ÍæUAV  
RWTexture2D<float4> OutlineAlbedo : register(u2);
RWTexture2D<float4> OutlineEmissive : register(u3);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    Albedo[DTid.xy] += OutlineAlbedo[DTid.xy];
    Emissive[DTid.xy] += OutlineEmissive[DTid.xy];
}