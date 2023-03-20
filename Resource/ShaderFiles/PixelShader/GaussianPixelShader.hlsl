#include"../ShaderHeader/SpriteShaderHeader.hlsli"

cbuffer PostEffect : register(b1)
{
    float4 bkweights[2];
}
Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);


float4 GaussianUpBlur(VSOutput input) : SV_TARGET
{
			//îwåiÇ∆ìØÇ∂êFÇ»ÇÁìßâﬂÇ∑ÇÈ
    float4 texCheck = tex.Sample(smp, input.uv);
    float r = texCheck.r;
    float g = texCheck.g;
    float b = texCheck.b;
	

	
    float w, h, levels;
    tex.GetDimensions(0, w, h, levels);

    float dx = 1.0f / w;
    float dy = 1.0f / h;
    float4 ret = float4(0, 0, 0, 0);
    float4 col = tex.Sample(smp, input.uv);
	

    ret += bkweights[0] * col;

    for (int i = 1; i < 8; ++i)
    {
        ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, input.uv + float2(0, dy * i));

        ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, input.uv + float2(0, dy * -i));
    }  

    return float4(ret.rgb, col.a);
	//return col;
    
}


float4 GaussianSideBlur(VSOutput input) : SV_TARGET
{
		//îwåiÇ∆ìØÇ∂êFÇ»ÇÁìßâﬂÇ∑ÇÈ
    float4 texCheck = tex.Sample(smp, input.uv);
    float r = texCheck.r;
    float g = texCheck.g;
    float b = texCheck.b;
    //if (color.r == r && color.g == g && color.b == b)
    //{
    //    return float4(0, 0, 0, 0);
    //}
	
	

 
	
    float w, h, levels;
    tex.GetDimensions(0, w, h, levels);

    float dx = 1.0f / w;
    float dy = 1.0f / h;
    float4 ret = float4(0, 0, 0, 0);
    float4 col = tex.Sample(smp, input.uv);


    ret += bkweights[0] * col;
	


    for (int i = 1; i < 8; ++i)
    {
        ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, input.uv + float2(dx * i, 0));
        ret += bkweights[i >> 2][i % 4] * tex.Sample(smp, input.uv + float2(dx * -i, 0));
    }

    return float4(ret.rgb, col.a);
    
}