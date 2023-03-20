#include"../ShaderHeader/OBJShaderHeader.hlsli"
#include"../ShaderHeader/BaseShaderHeader.hlsli"
#include"../ShaderHeader/ExpantionShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 PSmain(VSOutput input) : SV_TARGET
{
    float3 light = normalize(float3(1, -1, 1)); //右下奥向きのライト
    float light_diffuse = saturate(dot(-light, input.normal)); //環境光...diffuseを[0,1]の範囲にClampする

    float3 shade_color;
    shade_color = m_ambient + 0.5; //アンビエント
    shade_color += m_diffuse * light_diffuse; //ディフューズ項

    float4 texColor = tex.Sample(smp, input.uv);

	//return float4(texColor.rgb * shade_color, texColor.a * m_alpha);

    float4 returnBuff = float4(texColor.rgb, m_alpha);

    // テクスチャの色は使わない。
    returnBuff.rgba = 1.0f;

    returnBuff *= outlineColor;

    return returnBuff;
}