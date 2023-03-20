#include"../ShaderHeader/OBJShaderHeader.hlsli"
#include"../ShaderHeader/BaseShaderHeader.hlsli"
#include"../ShaderHeader/MultiPassHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

TwoRender PSmain(VSOutput input)
{
    float3 light = normalize(float3(1, -1, 1)); //右下奥向きのライト
    float light_diffuse = saturate(dot(-light, input.normal)); //環境光...diffuseを[0,1]の範囲にClampする
	
    float3 shade_color;
    shade_color = m_ambient + 0.5; //アンビエント
    shade_color += m_diffuse * light_diffuse; //ディフューズ項

    float4 texColor = tex.Sample(smp, input.uv);

    TwoRender output;
    output.target0 = float4(texColor.rgb, m_alpha);
    //output.target0 = float4(1,1,1,1);
    output.target1 = float4(0.0f, 0.0f, 0.0f, 0.0f);
    return output;
}