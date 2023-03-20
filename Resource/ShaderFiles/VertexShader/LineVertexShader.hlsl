#include"../ShaderHeader/BasicShaderHeader.hlsli"

VSOutputLine VSmain(float4 pos : POSITION, float2 tickness : TEXCOORD)
{
    VSOutputLine op;
    op.svpos = mul(world, pos); // モデルの頂点をワールド座標系に変換
    op.svpos = mul(view, op.svpos); // ワールド座標系からカメラ座標系に変換
    op.svpos = mul(viewproj, op.svpos); // カメラ座標系からスクリーン座標系に変換

    op.tickness = tickness;
    return op;
}