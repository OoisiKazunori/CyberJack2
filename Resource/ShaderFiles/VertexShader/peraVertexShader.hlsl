#include"../ShaderHeader/peraHeader.hlsli"


Output VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	Output op;

	op.svpos = mul(mat,pos);
	op.svpos = pos;
    op.uv = uv;

	return op;
}