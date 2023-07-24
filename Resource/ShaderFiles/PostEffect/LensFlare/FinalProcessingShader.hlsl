
//“ü—Íî•ñ
RWTexture2D<float4> InputImg : register(u0);

//o—ÍæUAV  
RWTexture2D<float4> OutputImg : register(u1);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    OutputImg[DTid.xy] = InputImg[DTid.xy];
    
}