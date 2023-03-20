#include"../ShaderHeader/BasicShaderHeader.hlsli"

VSOutput VSmain(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	VSOutput op;
	op.svpos = mul(mat, pos);
	op.normal = normal;
	op.uv = uv;

	return op;
}