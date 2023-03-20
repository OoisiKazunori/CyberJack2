#include"../ShaderHeader/FogShaderHeader.hlsli"
#include"../ShaderHeader/BaseShaderHeader.hlsli"
#include"../ShaderHeader/FogBufferHeader.hlsli"

FogObjOutput VSmain(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    FogObjOutput op;
    op.svpos = mul(world, pos); // モ�?ルの頂点をワールド座標系に変換
    op.fogFactor = op.svpos.xyz;
    op.svpos = mul(view, op.svpos); // ワールド座標系からカメラ座標系に変換
    op.svpos = mul(viewproj, op.svpos); // カメラ座標系からスクリーン座標系に変換
    op.uv = uv;
    op.normal = normal;
    
    return op;
}