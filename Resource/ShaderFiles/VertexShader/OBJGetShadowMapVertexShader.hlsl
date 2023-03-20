#include"../ShaderHeader/OBJShaderHeader.hlsli"
#include"../ShaderHeader/BaseShaderHeader.hlsli"

cbuffer cbuff2 : register(b2)
{
    matrix lightCameraViewProj;
}

VSOutput VSmain(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{

    VSOutput op;
    op.svpos = mul(mul(lightCameraViewProj, world), pos);
    //op.svpos = mul(mat, pos);
    op.normal = normal;
    op.uv = uv;
    return op;

}