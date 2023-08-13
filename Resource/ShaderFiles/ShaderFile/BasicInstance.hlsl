struct OutputData
{
    matrix mat;
    float4 color;
};
struct VertexBufferData
{
    float3 svpos;
    float3 normal;
    float2 uv;
    float3 tangent;
    float3 binormal;
};

RWStructuredBuffer<OutputData> matrixData : register(u0);
RWStructuredBuffer<VertexBufferData> vertexData : register(u1);

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float3 normal : NORMAL;
    float4 worldpos : WORLDPOS;
    float4 color : COLOR;
};

VSOutput VSmain(float4 pos : POSITION,uint id : SV_INSTANCEID)
{
    VSOutput op;
    op.worldpos = pos;
	op.svpos = mul(matrixData[id].mat, pos);
    op.color = matrixData[id].color;
    op.normal = vertexData[id].normal;
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
    output.normal = float4(input.normal,1.0f);
    output.metalnessRoughness = float4(0,1,0,1);
    output.world = input.worldpos;
    return output;
}