#define REGISTER_DRAW b0

cbuffer cbuff0 : register(REGISTER_DRAW)
{
    float4 color; //色
    matrix mat; //3D変換行列
    matrix viewproj;
    matrix world;
    matrix view;
}

cbuffer cbuff1 : register(b1)
{
    float3 m_ambient : packoffset(c0);
    float3 m_diffuse : packoffset(c1);
    float3 m_specular : packoffset(c2);
    float m_alpha : packoffset(c2.w);
}

cbuffer cbuff2 : register(b2)
{
    matrix lightCameraViewProj;
}

struct VSOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float3 normal : NORMAL; //法線ベクトル
    float2 uv : TEXCOORD; //uv値
    float4 posInLVP : TEXCOORD1;//ライトビュースクリーン空間でのピクセル座標
};