struct ColorOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float2 uv : TEXCOORD;    
};

cbuffer MatBuffer : register(b0)
{
    matrix mat; //3D変換行列
}

ColorOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    ColorOutput op;
    op.svpos = mul(mat,pos);
    op.uv = uv;
    return op;
}

Texture2D<float4> albedoGBuffer : register(t0);
SamplerState smp : register(s0);

float4 PSmain(ColorOutput input) : SV_TARGET
{
    float4 output = albedoGBuffer.Sample(smp,input.uv);
    return output;
}