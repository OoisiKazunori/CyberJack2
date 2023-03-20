#include"../ShaderHeader/OBJShaderHeader.hlsli"
#include"../ShaderHeader/BasicShaderHeader.hlsli"

float4 PSmain(ColorOutPut input) : SV_TARGET
{
    return float4(input.svpos.z, input.svpos.z, input.svpos.z, 1);
}