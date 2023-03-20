#include"../ShaderHeader/BasicShaderHeader.hlsli"
#include"../ShaderHeader/InstanceHeader.hlsli"

InstancePosNormalColorOut VSmain(float4 pos : POSITION, float3 normal : NORMAL,float2 uv : TEXCOORD, uint id : SV_InstanceID)
{
    InstancePosNormalColorOut op;
    op.svpos = mul(uavMatColorData[id].mat, pos);
    op.normal = normal;
    op.color = uavMatColorData[id].color;
    return op;
}