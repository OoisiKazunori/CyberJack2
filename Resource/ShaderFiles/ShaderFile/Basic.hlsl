struct OutputData
{
    matrix mat;
    float4 color;
};

cbuffer MatrixBuffer : register(b0)
{
    matrix mat;
}
cbuffer MatrixBuffer : register(b0)
{
    float4 color;
}

struct VSOutput
{
    float4 svpos : SV_POSITION;
};

VSOutput VSmain(float4 pos : POSITION)
{
	VSOutput op;
	op.svpos = mul(mat, pos);
	return op;
}

float4 PSmain(VSOutput input) : SV_TARGET
{
    return color;
}