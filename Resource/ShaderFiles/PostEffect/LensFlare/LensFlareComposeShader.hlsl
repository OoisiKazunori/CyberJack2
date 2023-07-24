
//“ü—Íî•ñ
RWTexture2D<float4> SceneImg : register(u0);

//“ü—Íî•ñ
RWTexture2D<float4> LensFlareImg : register(u1);

//o—ÍæUAV  
RWTexture2D<float4> OutputImg : register(u2);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    OutputImg[DTid.xy] = SceneImg[DTid.xy] + LensFlareImg[DTid.xy];
    
}