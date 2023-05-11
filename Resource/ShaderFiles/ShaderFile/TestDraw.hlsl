/*
単色表示の処理
*/

//共通処理---------------------------------------
cbuffer MatBuffer : register(b0)
{
    matrix mat; //3D変換行列
}
cbuffer ColorBuffer : register(b1)
{
    float4 color; //色
}
//共通処理---------------------------------------



//単色対応---------------------------------------
struct ColorOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float4 color : COLOR;
};    

ColorOutput VSmain(float4 pos : POSITION)
{
    ColorOutput op;
    op.svpos = mul(mat,pos);
    op.color = color;
    return op;
}

float4 PSmain(ColorOutput input) : SV_TARGET
{
    return input.color;
}
//単色対応---------------------------------------



//色、法線対応---------------------------------------
struct ColorNormalOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float4 color : COLOR;
    float3 normal : NORMAL; //法線ベクトル
};

ColorNormalOutput VSPosNormalmain(float4 pos : POSITION,float3 normal : NORMAL)
{
    ColorNormalOutput op;
    op.svpos = mul(mat,pos);
    op.color = color;
    op.normal = normal;
    return op;
}

float4 PSPosNormalmain(ColorNormalOutput input) : SV_TARGET
{
    float3 light = normalize(float3(1, -1, 1));
    float diffuse = saturate(dot(-light, input.normal));
    float brightness = diffuse + 0.3f;

    return input.color * float4(brightness, brightness, brightness, 1);
}
//色、法線対応---------------------------------------