//スプライト用のシェーダーヘッダー
cbuffer cbuff0 : register(b0)
{
    float4 color; //色
    matrix mat; //3D変換行列
}

struct VSOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float2 uv : TEXCOORD; //uv値
};

struct ClipTexOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float4 screenPos : TEXCOORD; //uv値
};

float4 ComputeNonStereoScreenPos(float4 pos)
{
    float4 o = pos * 0.5f;
    o.xy = float2(o.x, o.y * -1.0) + o.w;
    o.zw = pos.zw;
    return o;
};