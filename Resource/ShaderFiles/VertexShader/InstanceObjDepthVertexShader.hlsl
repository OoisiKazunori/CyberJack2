#include"../ShaderHeader/BasicShaderHeader.hlsli"
#include"../ShaderHeader/InstanceHeader.hlsli"

InstanceDepthOutPut VSmain(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, uint id : SV_InstanceID)
{
    InstanceDepthOutPut op;
    op.svpos = mul(uavMatData[id].matData, pos);
    op.normal = normal;
    op.uv = uv;
    op.id = id;
    op.worldPos = op.svpos;
    return op;
}