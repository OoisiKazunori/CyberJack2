#include"../ShaderHeader/SpriteShaderHeader.hlsli"

VSOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	VSOutput op;
	op.svpos = pos;
	op.uv = uv;
	
	return op;
}