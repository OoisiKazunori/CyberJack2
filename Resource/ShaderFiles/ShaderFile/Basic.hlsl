struct OutputData
{
    matrix mat;
    float4 color;
};
RWStructuredBuffer<OutputData> matrixData : register(u0);

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float4 color : COLOR;
};

VSOutput VSmain(float4 pos : POSITION,uint id : SV_INSTANCEID)
{
	VSOutput op;
	op.svpos = mul(matrixData[id].mat, pos);
    op.color = matrixData[id].color;
	return op;
}

float4 PSmain(VSOutput input) : SV_TARGET
{  
    return input.color;
}