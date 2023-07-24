
//入力情報
RWTexture2D<float4> InputImg : register(u0);

//出力先UAV  
RWTexture2D<float4> OutputImg : register(u1);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    OutputImg[DTid.xy] = InputImg[DTid.xy];
    
}