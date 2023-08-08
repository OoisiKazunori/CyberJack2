struct VSOutput
{
    float4 svpos : SV_POSITION;
};

cbuffer MatrixBuffer : register(b0)
{
    matrix vierProjMat;
}
cbuffer ColorBuffer : register(b1)
{
    float4 color;
}

VSOutput VSmain(float4 pos : POSITION)
{
	VSOutput op;
    op.svpos = mul(vierProjMat, pos);
	return op;
}

float4 PSmain(VSOutput input) : SV_TARGET
{   
    return color;
}