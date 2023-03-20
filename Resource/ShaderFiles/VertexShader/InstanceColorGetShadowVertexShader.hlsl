#include"../ShaderHeader/OBJShaderHeader.hlsli"
#include"../ShaderHeader/InstanceHeader.hlsli"
#include"../ShaderHeader/BasicShaderHeader.hlsli"

struct InstanceMatData
{
    matrix lightCameraViewProj;
};

cbuffer cbuff2 : register(b0)
{
    InstanceMatData lightCameraViewProj[INSTANCE_DATA_MAX];
}

ColorOutPut VSmain(float4 pos : POSITION, uint id : SV_InstanceID)
{
    ColorOutPut op;
    op.svpos = mul(mul(lightCameraViewProj[id].lightCameraViewProj, uavMatData[id].matData), pos);
    return op;
}