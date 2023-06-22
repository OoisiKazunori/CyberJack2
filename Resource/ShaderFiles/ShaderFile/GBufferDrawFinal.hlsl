struct VSOutput
{
    float4 svpos : SV_POSITION; //�V�X�e���p���_���W
    float2 uv : TEXCOORD; //uv�l
};

cbuffer MatBuffer : register(b0)
{
    matrix Mat; //3D�ϊ��s��
}

cbuffer LightDir : register(b1)
{
    float3 LightWorldPos;
    float3 attenVec;
}

VSOutput VSmain(float4 pos : POSITION, float2 uv : TEXCOORD)
{
	VSOutput op;
	op.svpos = mul(Mat, pos);
	op.uv = uv;
	return op;
}

Texture2D<float4> AlbedoTex : register(t0);
Texture2D<float4> NormalTex : register(t1);
Texture2D<float4> WorldTex : register(t2);
RWTexture2D<float4> finalTex : register(u0);
SamplerState smp : register(s0);

float4 PSmain(VSOutput input) : SV_TARGET
{
    float4 albedoColor = AlbedoTex.Sample(smp, input.uv);
    float4 worldNormalVec = NormalTex.Sample(smp, input.uv);
    float4 worldPos = WorldTex.Sample(smp, input.uv);

    //���C�g�̃x�N�g���v�Z
    float3 lightV = LightWorldPos - worldPos.xyz;
    float d = length(lightV);
    lightV = normalize(lightV);

    //���������W��
    float atten = 400.0f / (attenVec.x + attenVec.y * d + attenVec.z * d * d);
    float bright = dot(lightV,worldNormalVec.xyz);
        
    float3 lightColor = float3(1.0f,1.0f,1.0f);
    
    float ambient = 0.2f;
    float3 light = (bright * atten + ambient) * lightColor;
    light = saturate(light);

    //����
    float4 outputColor = albedoColor;

    finalTex[input.uv * uint2(1280,720)] = float4(outputColor.xyz * light,outputColor.a);
    return float4(outputColor.xyz * light,outputColor.a);
}