#include"../ShaderHeader/BasicShaderHeader.hlsli"

ColorOutPut VSmain(float4 pos : POSITION)
{
    ColorOutPut op;
    op.svpos = mul(world, pos); // モデルの頂点をワールド座標系に変換
    op.svpos = mul(view, op.svpos); // ワールド座標系からカメラ座標系に変換
    op.svpos = mul(viewproj, op.svpos); // カメラ座標系からスクリーン座標系に変換

    return op;
}