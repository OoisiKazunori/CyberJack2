#include"../ShaderHeader/OBJShaderHeader.hlsli"
#include"../ShaderHeader/BaseShaderHeader.hlsli"

VSLightOutput VSmain(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    float4 wNormal = normalize(mul(world, float4(normal, 0)));
    float4 wPos = mul(world, pos);
    
    VSLightOutput op;
    op.svpos = mul(mat, pos);
    op.normal = wNormal.xyz;
    op.worldPos = wPos;
    op.uv = uv;
    return op;
}