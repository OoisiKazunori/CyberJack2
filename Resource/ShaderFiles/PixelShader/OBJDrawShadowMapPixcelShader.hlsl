#include"../ShaderHeader/OBJDrawShadowMapSHaderHeader.hlsli"

Texture2D<float4> tex : register(t0);
Texture2D<float4> shadowMap : register(t1);
SamplerState smp : register(s0);

float4 PSmain(VSOutput input) : SV_TARGET
{

    // ライトビュースクリーン空間空間からUV座標空間に座標変換
    float2 shadowMapUV = input.posInLVP.xy / input.posInLVP.w;
    shadowMapUV *= float2(0.5f, -0.5f);
    shadowMapUV += 0.5f;

    // ライトビュースクリーン空間でのZ値を計算する。
    float zInLVP = input.posInLVP.z / input.posInLVP.w;

    // 取得したシャドウマップUV座標を使って影情報をサンプリング
    float3 shadowMapColor = 1.0f;
    if (shadowMapUV.x > 0.0f && shadowMapUV.x < 1.0f
        && shadowMapUV.y > 0.0f && shadowMapUV.y < 1.0f)
    {

        // シャドウマップに書き込まれているZ値を比較をする。
        float zInShadowMap = shadowMap.Sample(smp, shadowMapUV).r;

        if (zInLVP > zInShadowMap)
        {

            // 隠れている。
            shadowMapColor = 100.0f;

        }

    }

    float3 light = normalize(float3(1, -1, 1)); //右下奥向きのライト
    float light_diffuse = saturate(dot(-light, input.normal)); //環境光...diffuseを[0,1]の範囲にClampする

    float3 shade_color;
    shade_color = m_ambient + 0.5; //アンビエント
    shade_color += m_diffuse * light_diffuse; //ディフューズ項

    float4 texColor = tex.Sample(smp, input.uv);

    //return float4(texColor.rgb * shade_color, texColor.a * m_alpha);
    float4 returnBuff = float4(texColor.rgb, m_alpha);

    // 影の色を強制的に変えるために、隠れているところの色を100にしてその場合は指定の色を無理やり入れる。
    if (shadowMapColor.r >= 100)
    {
        returnBuff.xyz = float3(0x32 / 255.0f, 0x2f / 255.0f, 0x2e / 255.0f);
    }

    return returnBuff;
}