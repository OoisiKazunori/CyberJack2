#include"../ShaderHeader/BasicShaderHeader.hlsli"

VSOutput VSmain( float4 pos : POSITION, float4 normal : NORMAL,float2 uv: TEXCOORD,
			   min16uint2 boneno:BONENO,min16uint weight :WEIGHT)
{
	VSOutput op;
	op.svpos = mul(mul(viewproj, mat), pos);//シェーダーでは列優先
	normal.w = 0;
	op.normal = mul(mat, normal);
	op.uv = uv;

	return op;
}