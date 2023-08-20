struct VSOutput
{
    float4 svpos : SV_POSITION;
};

cbuffer MatBuffer : register(b0)
{
    matrix mat;
}

cbuffer ColorBuffer : register(b1)
{
    float4 color;
}

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