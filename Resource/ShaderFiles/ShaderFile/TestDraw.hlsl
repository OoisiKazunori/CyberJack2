struct ColorOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float4 color : COLOR;
};

cbuffer MatBuffer : register(b0)
{
    matrix mat; //3D変換行列
}
cbuffer ColorBuffer : register(b1)
{
    float4 color; //色
}

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