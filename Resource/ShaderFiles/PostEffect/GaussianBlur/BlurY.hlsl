
//入力情報
RWTexture2D<float4> InputImg : register(u0);

//出力先UAV  
RWTexture2D<float4> OutputImg : register(u1);

//重みテーブル
cbuffer GaussianWeight : register(b0)
{
    float4 weights[2];
};

float4 GetPixelColor(int x, int y, int2 texSize)
{
    x = clamp(0, texSize.x, x);
    y = clamp(0, texSize.y, y);

    return InputImg[uint2(x, y)];
}

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    uint2 basepos = uint2(DTid.x, DTid.y * 2);
    
    float4 color;
    
    color = GetPixelColor(basepos.x, basepos.y, float2(1280 / 2.0f, 720)) * weights[0].x;
    color += GetPixelColor(basepos.x, basepos.y + 1, float2(1280 / 2.0f, 720)) * weights[0].y;
    color += GetPixelColor(basepos.x, basepos.y + 2, float2(1280 / 2.0f, 720)) * weights[0].z;
    color += GetPixelColor(basepos.x, basepos.y + 3, float2(1280 / 2.0f, 720)) * weights[0].w;
    color += GetPixelColor(basepos.x, basepos.y + 4, float2(1280 / 2.0f, 720)) * weights[1].x;
    color += GetPixelColor(basepos.x, basepos.y + 5, float2(1280 / 2.0f, 720)) * weights[1].y;
    color += GetPixelColor(basepos.x, basepos.y + 6, float2(1280 / 2.0f, 720)) * weights[1].z;
    color += GetPixelColor(basepos.x, basepos.y + 7, float2(1280 / 2.0f, 720)) * weights[1].w;
    
    color += GetPixelColor(basepos.x, basepos.y, float2(1280 / 2.0f, 720)) * weights[0].x;
    color += GetPixelColor(basepos.x, basepos.y - 1, float2(1280 / 2.0f, 720)) * weights[0].y;
    color += GetPixelColor(basepos.x, basepos.y - 2, float2(1280 / 2.0f, 720)) * weights[0].z;
    color += GetPixelColor(basepos.x, basepos.y - 3, float2(1280 / 2.0f, 720)) * weights[0].w;
    color += GetPixelColor(basepos.x, basepos.y - 4, float2(1280 / 2.0f, 720)) * weights[1].x;
    color += GetPixelColor(basepos.x, basepos.y - 5, float2(1280 / 2.0f, 720)) * weights[1].y;
    color += GetPixelColor(basepos.x, basepos.y - 6, float2(1280 / 2.0f, 720)) * weights[1].z;
    color += GetPixelColor(basepos.x, basepos.y - 7, float2(1280 / 2.0f, 720)) * weights[1].w;
    
    
    color = (color);
    
    OutputImg[DTid.xy] = color;
    
}