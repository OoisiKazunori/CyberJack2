#include"../ShaderHeader/SpriteShaderHeader.hlsli"

Texture2D<float4>tex1:register(t0);
Texture2D<float4>tex2:register(t1);
Texture2D<float4>tex3:register(t2);
Texture2D<float4>tex4:register(t3);

SamplerState smp :register(s0);

float4 PSmain(VSOutput input) : SV_TARGET
{
	float4 combineColor = tex1.Sample(smp, input.uv);
	combineColor += tex2.Sample(smp, input.uv);
	combineColor += tex3.Sample(smp, input.uv);
	combineColor += tex4.Sample(smp, input.uv);
	//combineColor /= 7.0f;
	combineColor.a = color.a;
	
	return combineColor;
}
