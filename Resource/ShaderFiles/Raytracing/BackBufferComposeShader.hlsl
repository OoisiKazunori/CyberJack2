
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
    bool isBackGroundColor = BackBuffer[DTid.xy].a <= 0.0f;
    if (isBackGroundColor)
    {
    
        OutputImg[DTid.xy] = Raytracing[DTid.xy];
        
    }
    else
    {
        float alpha = BackBuffer[DTid.xy].w;
        OutputImg[DTid.xy] = BackBuffer[DTid.xy] * alpha + Raytracing[DTid.xy] * (1.0f - alpha);
    
    }
    
}