#include"../ShaderHeader/BasicShaderHeader.hlsli"
#include"../ShaderHeader/InstanceHeader.hlsli"

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 PSmain(InstanceOutPut input) : SV_TARGET
{
    float3 light = normalize(float3(1, -1, 1)); //‰E‰º‰œŒü‚«‚Ìƒ‰ƒCƒg
    float light_diffuse = saturate(dot(-light, input.normal)); //ŠÂ‹«Œõ...diffuse‚ð[0,1]‚Ì”ÍˆÍ‚ÉClamp‚·‚é

    float4 texColor = tex.Sample(smp, input.uv);

	//return float4(texColor.rgb * shade_color, texColor.a * m_alpha);
    return float4(texColor.rgb, 1.0f);
}