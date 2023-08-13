//“ü—Íî•ñ
RWTexture2D<float4> InputImg0 : register(u0);
RWTexture2D<float4> InputImg1 : register(u1);
RWTexture2D<float4> InputImg2 : register(u2);
RWTexture2D<float4> InputImg3 : register(u3);
RWTexture2D<float4> InputImg4 : register(u4);

//o—ÍæUAV  
RWTexture2D<float4> OutputImg : register(u5);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    float4 finalColor = InputImg0[DTid.xy] + InputImg1[DTid.xy] + InputImg2[DTid.xy] + InputImg3[DTid.xy] + InputImg4[DTid.xy];
    
    OutputImg[DTid.xy] = finalColor;
    
}