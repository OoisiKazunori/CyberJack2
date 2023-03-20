#include"../ShaderHeader/BasicShaderHeader.hlsli"
#include"../ShaderHeader/InstanceHeader.hlsli"

InstancePosOut VSmain(float4 pos : POSITION, uint id : SV_InstanceID)
{
    InstancePosOut op;
    op.svpos = mul(uavMatColorData[id].mat, pos);
    op.id = id;
    return op;
}