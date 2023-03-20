#define REGISTER_DRAW b0

cbuffer BaseBuffer : register(REGISTER_DRAW)
{
    float4 color; //色
    matrix mat; //3D変換行列
    matrix viewproj;
    matrix world;
    matrix view;
}

struct VSOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float3 normal : NORMAL; //法線ベクトル
    float2 uv : TEXCOORD; //uv値
};