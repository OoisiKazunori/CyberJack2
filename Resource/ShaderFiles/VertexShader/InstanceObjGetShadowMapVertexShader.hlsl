#include"../ShaderHeader/OBJShaderHeader.hlsli"
#include"../ShaderHeader/BaseShaderHeader.hlsli"
#include"../ShaderHeader/InstanceHeader.hlsli"


struct InstanceMatData
{
    matrix lightCameraViewProj;
};

cbuffer cbuff2 : register(b2)
{
    InstanceMatData lightCameraViewProj[INSTANCE_DATA_MAX];
}

VSOutput VSmain(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, uint id : SV_InstanceID)
{
    VSOutput op;
    op.svpos = mul(mul(lightCameraViewProj[id].lightCameraViewProj, uavMatData[id].matData), pos);
    op.normal = normal;
    op.uv = uv;
    return op;
}