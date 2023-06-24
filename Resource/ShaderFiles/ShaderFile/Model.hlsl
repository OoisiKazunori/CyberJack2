#include"ModelBuffer.hlsli"

cbuffer MatBuffer : register(b0)
{
    matrix worldMat;
    matrix viewMat;
    matrix projectionMat;
}

cbuffer LightBufferB1 : register(b1)
{
    float3 localLightDir;
}

//uv�A�@���Ή�---------------------------------------
struct PosUvNormalOutput
{
    float4 svpos : SV_POSITION; //�V�X�e���p���_���W
    float3 normal : NORMAL; //�@���x�N�g��
    float2 uv : TEXCOORD;
    float3 lightInTangentWorld : TANGENT;
    float3 worldPos : POSITION;
};

PosUvNormalOutput VSPosNormalUvmain(float4 pos : POSITION,float3 normal : NORMAL,float2 uv:TEXCOORD,float3 tangent : TANGENT,float3 binormal : BINORMAL)
{
    PosUvNormalOutput op;
    op.svpos = mul(worldMat,pos);
    op.worldPos = op.svpos;
    op.svpos = mul(viewMat,op.svpos);
    op.svpos = mul(projectionMat,op.svpos);
    op.uv = uv;
    op.normal = normal;

    float4 lightDir = float4(localLightDir.xyz,0.0f);
    lightDir = normalize(lightDir);
    //���[�J����Ԃɂ��郉�C�g���^���W�F���g��ԂɈړ�������
    op.lightInTangentWorld = mul(lightDir,InvTangentMatrix(tangent,binormal,normal));

    return op;
}

float4 PSPosNormalUvmain(PosUvNormalOutput input) : SV_TARGET
{
    float3 normalColor = NormalTex.Sample(smp,input.uv);
    //-�\��������ׂɁA0~255�̔�����0.0f�n�_�ɂ���悤�v�Z����B
    //-1.0f ~ 1.0f
    float3 normalVec = 2 * normalColor - 1.0f;
    normalVec = normalize(normalVec);

    //�^���W�F���g��Ԃ̃x�N�g��������
    float3 bright = dot(input.lightInTangentWorld,normalVec);
    //bright = max(0.0f,bright);

	float4 texColor = AlbedoTex.Sample(smp,input.uv);
	return float4(texColor.rgb * bright, 1.0f);
}
//uv�A�@���Ή�---------------------------------------


//���f���AG-Buffer�i�[---------------------------------------

cbuffer LightBufferB2 : register(b2)
{
    float3 localLightDirB2;
}

struct PosUvNormalTangentBinormalOutput
{
    float4 svpos : SV_POSITION; //�V�X�e���p���_���W
    float3 normal : NORMAL; //�@���x�N�g��
    float2 uv : TEXCOORD;
    float3 lightInTangentWorld : TANGENT;
    float3 worldPos : POSITION;
    float3 tangent : TANGENT2;
    float3 binormal : BINORMAL;
};

PosUvNormalTangentBinormalOutput VSDefferdMain(float4 pos : POSITION,float3 normal : NORMAL,float2 uv:TEXCOORD,float3 tangent : TANGENT,float3 binormal : BINORMAL)
{
    PosUvNormalTangentBinormalOutput op;
    op.svpos = mul(worldMat,pos);
    op.worldPos = op.svpos;
    op.svpos = mul(viewMat,op.svpos);
    op.svpos = mul(projectionMat,op.svpos);
    op.uv = uv;
    op.normal = normal;
    op.binormal = binormal;
    op.tangent = tangent;

    float4 lightDir = float4(localLightDirB2.xyz,0.0f);
    lightDir = normalize(lightDir);
    //���[�J����Ԃɂ��郉�C�g���^���W�F���g��ԂɈړ�������
    op.lightInTangentWorld = mul(lightDir,InvTangentMatrix(tangent,binormal,normal));

    return op;
}

struct GBufferOutput
{
    float4 albedo : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 metalnessRoughness : SV_TARGET2;
    float4 world : SV_TARGET3;
    float4 final : SV_TARGET4;
};

GBufferOutput PSDefferdMain(PosUvNormalTangentBinormalOutput input) : SV_TARGET
{
    float4 normalColor = NormalTex.Sample(smp,input.uv);
    //-�\��������ׂɁA0~255�̔�����0.0f�n�_�ɂ���悤�v�Z����B
    //-1.0f ~ 1.0f
    float3 normalVec = 2 * normalColor - 1.0f;
    normalVec = normalize(normalVec);

    //�^���W�F���g��Ԃ̃x�N�g��������
    float3 bright = dot(input.lightInTangentWorld,normalVec);
    //�A���x�h
	float4 texColor = AlbedoTex.Sample(smp,input.uv);
    //���^���A���t
    float4 mrColor = MetalnessRoughnessTex.Sample(smp,input.uv);

    //���[���h��Ԃ̖@��
    float3 normal = mul(worldMat,float4(input.normal,1.0f));
    float3 tangent = mul(worldMat,float4(input.tangent,1.0f));
    float3 binormal = cross(normal,tangent);


    //�^���W�F���g��Ԃ��烍�[�J����Ԃ̖@���ɒ�����
    float3 nWorld = CalucurateTangentToLocal(normalVec,normal,tangent,binormal);
    bright = dot(normalize(localLightDirB2),nWorld);

    if(IsEnableToUseMaterialTex(mrColor))
    {
        mrColor.xyz = float3(-1,-1,-1)
    }

    GBufferOutput output;
    output.albedo = texColor;
    output.normal = float4(nWorld,normalColor.a);
    output.metalnessRoughness = float4(mrColor.xyz,raytracingId);
    output.world = float4(input.worldPos,1.0f);
    output.final = float4(texColor.xyz * bright,texColor.a);
	return output;
}

//���f���AG-Buffer�i�[---------------------------------------