struct GBufferOutput
{
    float4 albedo : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 metalnessRoughness : SV_TARGET2;
    float4 world : SV_TARGET3;
    float4 emissive : SV_TARGET4;
};


struct VSOutput
{
    float4 svpos : SV_POSITION; //�V�X�e���p���_���W
    float2 uv : TEXCOORD; //uv�l
};

cbuffer MatBuffer : register(b0)
{
    matrix mat; //3D�ϊ��s��
}

cbuffer ColorBuffer : register(b1)
{
    float4 color; //3D�ϊ��s��
}

VSOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	VSOutput op;
	op.svpos = mul(mat, pos);
	op.uv = uv;
	return op;
}

Texture2D<float4> tex : register(t0);
SamplerState smp1 :register(s0);

//マルチパスレンダリング
GBufferOutput PSmain(VSOutput input) : SV_TARGET
{
    float4 textureColor = float4(tex.Sample(smp1, input.uv));    
    if (textureColor.w * color.w < 0.01f)
    {
        discard;
    }

    GBufferOutput output;
    output.albedo = textureColor * color;
    output.normal = float4(0,0,0,0);
    output.metalnessRoughness = float4(0,0,0,0);
    output.emissive = float4(0,0,0,0);
    output.world = float4(0,0,0,0);
    return output;
}