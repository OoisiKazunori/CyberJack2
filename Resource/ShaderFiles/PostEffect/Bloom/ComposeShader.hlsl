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
    float power[5] = { 0, 0, 0, 0, 0 };
    float powerFacter = 0.6f;
    for (int index = 4; 0 <= index; --index)
    {
        power[index] = powerFacter;
        powerFacter /= 2.0f;

    }
    float4 finalColor = InputImg0[DTid.xy] * power[0] + 
    InputImg1[DTid.xy] * power[1] + 
    InputImg2[DTid.xy] * power[2] + 
    InputImg3[DTid.xy] * power[3] + 
    InputImg4[DTid.xy] * power[4];
    
    OutputImg[DTid.xy] = finalColor;
    
}