cbuffer MatBuffer : register(b0)
{
    matrix mat; //3D�ϊ��s��
}

//uv�A�@���Ή�---------------------------------------
struct PosUvNormalOutput
{
    float4 svpos : SV_POSITION; //�V�X�e���p���_���W
    float3 normal : NORMAL; //�@���x�N�g��
    float2 uv : TEXCOORD;
};

PosUvNormalOutput VSPosNormalUvmain(float4 pos : POSITION,float3 normal : NORMAL,float2 uv:TEXCOORD)
{
    PosUvNormalOutput op;
    op.svpos = mul(mat,pos);
    op.uv = uv;
    op.normal = normal;
    return op;
}

Texture2D<float4>tex:register(t0);
SamplerState smp :register(s0);

float4 PSPosNormalUvmain(PosUvNormalOutput input) : SV_TARGET
{
	float4 texColor = tex.Sample(smp,input.uv);
	return float4(texColor.rgb, 1.0f);
}
//uv�A�@���Ή�---------------------------------------