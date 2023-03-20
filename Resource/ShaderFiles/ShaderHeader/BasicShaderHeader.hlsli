#include"../ShaderHeader/BaseShaderHeader.hlsli"

//struct VSOutput
//{
//    float4 svpos : SV_POSITION; //システム用頂点座標
//    float3 normal : NORMAL; //法線ベクトル
//    float2 uv : TEXCOORD; //uv値
//};

struct ColorOutPut
{
    float4 svpos : SV_POSITION; //システム用頂点座標
};

struct VSin
{
    float4 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VSOutputColor
{
    float4 svpos : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VSOutputLine
{
    float4 svpos : SV_POSITION;
    float2 tickness : TEXCOORD;
};