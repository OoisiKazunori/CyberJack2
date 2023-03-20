#include"../ShaderHeader/OBJShaderHeader.hlsli"
#include"../ShaderHeader/FogShaderHeader.hlsli"
#include"../ShaderHeader/bgHeader.hlsli"

struct VSOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float3 normal : NORMAL; //法線ベクトル
    float2 uv : TEXCOORD; //uv値
};

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

    //objに値が送れない為、定数を配置する
    float3 tmpC = float3(0.0862745121, 0.149019614, 0.145098045);
    float tmpD = 0.003;
    float4 fog = CaluFog(input.svpos, texColor, tmpC, tmpD);
    //float4 fog = CaluFog(input.svpos, texColor, fogData.xyz, fogData.w);
    
    return float4(fog.rgb, fog.a * m_alpha);
}