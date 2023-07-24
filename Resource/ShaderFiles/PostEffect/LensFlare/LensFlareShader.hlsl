
//“ü—Íî•ñ
RWTexture2D<float4> InputImg : register(u0);

//o—ÍæUAV  
RWTexture2D<float4> OutputImg : register(u1);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    uint2 basepos = uint2(DTid.x / 2, DTid.y / 2);
    
    OutputImg[DTid.xy] = InputImg[basepos];
    
}