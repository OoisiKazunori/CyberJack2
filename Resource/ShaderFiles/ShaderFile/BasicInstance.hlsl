#include"ModelBuffer.hlsli"

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

struct VSOutput
{
    float4 svpos : SV_POSITION;
    float3 normal : NORMAL;
    float3 worldpos : WORLDPOS;
    float4 color : COLOR;
};

struct VertexInput
{
    float3 svpos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

VSOutput VSmain(VertexInput input, uint id : SV_INSTANCEID)
{
    VSOutput op;
    op.worldpos = input.svpos;
    op.svpos = mul(matrixData[id].mat, float4(input.svpos, 1.0f));
    op.color = matrixData[id].color;
    op.normal = input.normal;
    return op;
}

GBufferOutput PSmain(VSOutput input) : SV_TARGET
{
    GBufferOutput output;
    output.albedo = input.color;
    output.normal = float4(-1, -1, -1, 1.0f); //正しいワールド座標が取れないため、ここで法線を-1,-1,-1にすることによってGPUパーティクルであるということをレイトレ側に伝え、アルベドで塗りつぶす処理を書いています。
    output.metalnessRoughness = float4(0, 1, 0, 1);
    output.world = float4(input.svpos.xyz, 1.0f);
    output.emissive = output.albedo / 2.0f;
    return output;
}