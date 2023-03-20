#include"../ShaderHeader/BaseShaderHeader.hlsli"

struct ClipTexUvOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float3 normal : NORMAL; //法線ベクトル
    float2 uv : TEXCOORD; //uv値
    float4 screenPos : TEXCOORD1; //uv値
};

float4 ComputeNonStereoScreenPos(float4 pos)
{
    float4 o = pos * 0.5f;
    o.xy = float2(o.x, o.y * -1.0) + o.w;
    o.zw = pos.zw;
    return o;
};

ClipTexUvOutput VSmain(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	ClipTexUvOutput op;
	op.svpos = mul(mat, pos);
    op.uv = uv;
    op.normal = normal;
	op.screenPos = ComputeNonStereoScreenPos(op.svpos);
	return op;
}