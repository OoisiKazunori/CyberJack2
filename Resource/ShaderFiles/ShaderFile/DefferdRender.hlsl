struct ColorOutput
{
    float4 svpos : SV_POSITION; //�V�X�e���p���_���W
    float4 color : COLOR;    
};

cbuffer MatBuffer : register(b0)
{
    matrix mat; //3D�ϊ��s��
}
cbuffer ColorBuffer : register(b1)
{
    float4 color; //�F
}

ColorOutput VSmain(float4 pos : POSITION)
{
    ColorOutput op;
    op.svpos = mul(mat,pos);
    op.color = color;
    return op;
}

RWTexture2D<float4> colorBuffer : register(u0);

float4 PSmain(ColorOutput input) : SV_TARGET
{
    colorBuffer[input.svpos.xy] = input.color;
    return input.color;
}