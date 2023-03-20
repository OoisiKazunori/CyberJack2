#include"../ShaderHeader/BasicShaderHeader.hlsli"

float4 PSmain(VSOutputLine input) : SV_TARGET
{
    float4 result = color;    
    result.a = color.a;
    return color;
}