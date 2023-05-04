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

ColorOutput VSmain(float4 pos : POSITION,float2 uv : TEXCOORD)
{
    ColorOutput op;
    op.svpos = mul(mat,pos);
    op.color = color;
    return op;
}

RWStructuredBuffer<float4> colorBuffer : register(u0);

float4 PSmain(ColorOutput input) : SV_TARGET
{
    uint2 index = input.svpos;
    colorBuffer[index.x][index.y] = input.svpos;
    return input.color;
}