struct VSOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float2 uv : TEXCOORD; //uv値
};

cbuffer MatBuffer : register(b0)
{
    matrix mat; //3D変換行列
}

VSOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	VSOutput op;
	op.svpos = mul(mat, pos);
	op.uv = uv;
	return op;
}

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 PSmain(VSOutput input) : SV_TARGET
{
    float4 output = float4(tex.Sample(smp, input.uv));    
    return output;
}