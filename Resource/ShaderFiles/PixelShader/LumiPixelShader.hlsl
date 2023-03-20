#include"../ShaderHeader/BasicShaderHeader.hlsli"
#include"../ShaderHeader/LightHeader.hlsli"

/////////////////////////////////////////////////////////
// 輝度抽出用
/////////////////////////////////////////////////////////
/*!
 * @brief 輝度抽出用のピクセルシェーダー
 */
float4 PSmain(VSOutput input) : SV_TARGET
{
    // step-14 輝度を抽出するピクセルシェーダーを実装
    float4 tex = mainRenderTargetTexture.Sample(Sampler,input.uv);

    float t = dot(tex.xyz, float3(0.2125f, 0.7154f, 0.0721f));

    //clip(t - 1.0f);
    return float4(t, t, t, 1);
    //return float4(input.uv, 1, 1);
}


