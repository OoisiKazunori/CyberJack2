
/*
分ける理由
1.構造体っぽく
2.使い分ける単位
例.
ライトとオブジェクト
ライトは一度送っただけで値をそのまま使えるが、オブジェクトは頻繁に切り替える
テクスチャの切り替えが負担かかる...
パーティクルだと一回セットして描画する
*/

cbuffer cbuff1 : register(b1)
{
    float3 m_ambient : packoffset(c0);
    float3 m_diffuse : packoffset(c1);
    float3 m_specular : packoffset(c2);
    float m_alpha : packoffset(c2.w);
}

//struct VSOutput
//{
//    float4 svpos : SV_POSITION; //システム用頂点座標
//    float3 normal : NORMAL; //法線ベクトル
//    float2 uv : TEXCOORD; //uv値
//};


struct VSLightOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float3 normal : NORMAL; //法線ベクトル
    float4 worldPos : POSITION; //法線ベクトル
    float2 uv : TEXCOORD; //uv値
};
