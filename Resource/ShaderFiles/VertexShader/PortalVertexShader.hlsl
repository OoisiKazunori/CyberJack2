#include"../ShaderHeader/SpriteShaderHeader.hlsli"

ClipTexOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	ClipTexOutput op;
	op.svpos = mul(mat, pos);
	op.screenPos = ComputeNonStereoScreenPos(op.svpos);
	return op;
}