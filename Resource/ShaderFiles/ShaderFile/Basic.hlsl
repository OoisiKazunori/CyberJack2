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

struct GBufferOutput
{
    float4 albedo : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 metalnessRoughness : SV_TARGET2;
    float4 world : SV_TARGET3;
};

GBufferOutput PSmain(VSOutput input) : SV_TARGET
{
    GBufferOutput output;
    output.albedo = input.color;
    output.normal = float4(0,0,0,0);
    output.metalnessRoughness = float4(0,0,0,0);
    output.world = float4(0,0,0,0);
    return output;
}