#include"../ShaderHeader/BasicShaderHeader.hlsli"
#include"../ShaderHeader/InstanceHeader.hlsli"

float4 PSmain(InstancePosOut input) : SV_TARGET
{
    return uavMatColorData[input.id].color;
}