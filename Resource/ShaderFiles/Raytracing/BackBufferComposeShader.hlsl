
//“ü—Íî•ñ
RWTexture2D<float4> BackBuffer : register(u0);

//“ü—Íî•ñ
RWTexture2D<float4> Raytracing : register(u1);

//o—ÍæUAV  
RWTexture2D<float4> OutputImg : register(u2);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    //”wŒiF‚¾‚Á‚½‚çƒŒƒCƒgƒŒ‚ÌŒ‹‰Ê‚ğ•`‰æ
    bool isBackGroundColor = 0.8f <= BackBuffer[DTid.xy].r && BackBuffer[DTid.xy].g <= 0.01f && BackBuffer[DTid.xy].b <= 0.01f;
    if (isBackGroundColor)
    {
    
        OutputImg[DTid.xy] = Raytracing[DTid.xy];
        
    }
    else
    {
        
        OutputImg[DTid.xy] = BackBuffer[DTid.xy];
    
    }
    
}