//入力情報
Texture2D<float4> OutlineMap : register(t0);

//出力先UAV  
RWTexture2D<float4> Albedo : register(u0);
RWTexture2D<float4> Emissive : register(u1);

cbuffer Dissolve : register(b0)
{
    float4 m_outlineColor;
}

float3 SamplingPixel(uint2 arg_uv)
{
    return OutlineMap[uint2(clamp(arg_uv.x, 0, 1280), clamp(arg_uv.y, 0, 720))].xyz;
}

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    //パティクルだったら光らせない。
    //何も無いところには描画しない。
    bool isParticle = OutlineMap[DTid.xy].x <= -0.9f && OutlineMap[DTid.xy].y <= -0.9f && OutlineMap[DTid.xy].z <= -0.9f;
    bool isNonObject = length(OutlineMap[DTid.xy].xyz) <= 0.1f;
    if (isParticle || isNonObject)
    {
        
        Albedo[DTid.xy] = float4(0, 0, 0, 0);
        Emissive[DTid.xy] = float4(0, 0, 0, 0);
        return;
        
    }
    
    //アウトラインを出すかの判定を取る。
    bool isRightPix = length(SamplingPixel(DTid.xy + uint2(2, 0))) <= 0.1f;
    bool isLeftPix = length(SamplingPixel(DTid.xy + uint2(-2, 0))) <= 0.1f;
    bool isUpPix = length(SamplingPixel(DTid.xy + uint2(0, 2))) <= 0.1f;
    bool isDownPix = length(SamplingPixel(DTid.xy + uint2(0, -2))) <= 0.1f;
    
    if (isRightPix || isLeftPix || isLeftPix || isDownPix)
    {

        Albedo[DTid.xy] = OutlineMap[DTid.xy];
        Albedo[DTid.xy].xyz *= Albedo[DTid.xy].w;
        Emissive[DTid.xy] = OutlineMap[DTid.xy];
        Emissive[DTid.xy].xyz *= Emissive[DTid.xy].w;
        
    }
    else
    {
        
        Albedo[DTid.xy] = float4(0, 0, 0, 0);
        Emissive[DTid.xy] = float4(0, 0, 0, 0);
        
    }
    

}