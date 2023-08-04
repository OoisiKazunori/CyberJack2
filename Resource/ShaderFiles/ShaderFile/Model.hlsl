#include"ModelBuffer.hlsli"

cbuffer MatBuffer : register(b0)
{
    matrix worldMat;
    matrix viewMat;
    matrix projectionMat;
    matrix rotaion;
}

cbuffer LightBufferB1 : register(b1)
{
    float3 localLightDir;
}

cbuffer ColorBuffer : register(b1)
{
    float4 colorB1;
}

struct PosUvNormalOutput
{
    float4 svpos : SV_POSITION; 
    float3 normal : NORMAL; 
    float2 uv : TEXCOORD;
    float3 worldPos : POSITION;
};

//ライトなし頂点変換
PosUvNormalOutput VSPosNormalUvmain(float4 pos : POSITION,float3 normal : NORMAL,float2 uv:TEXCOORD,float3 tangent : TANGENT,float3 binormal : BINORMAL)
{
    PosUvNormalOutput op;
    op.svpos = mul(worldMat,pos);
    op.worldPos = op.svpos.xyz;
    op.svpos = mul(viewMat,op.svpos);
    op.svpos = mul(projectionMat,op.svpos);
    op.uv = uv;
    op.normal = normal;
    return op;
}

//乗算ありPS
float4 PSPosNormalUvmain(PosUvNormalOutput input) : SV_TARGET
{
	float4 texColor = AlbedoTex.Sample(smp,input.uv);
    texColor *= colorB1;
	return float4(texColor.rgb, texColor.a);
}


struct ModelWithLightOutputData
{
    float4 svpos : SV_POSITION; 
    float3 normal : NORMAL; 
    float2 uv : TEXCOORD;
    float3 lightInTangentWorld : TANGENT;
    float3 worldPos : POSITION;
};

//ライトあり頂点返還
ModelWithLightOutputData VSPosNormalUvLightMain(float4 pos : POSITION,float3 normal : NORMAL,float2 uv:TEXCOORD,float3 tangent : TANGENT,float3 binormal : BINORMAL)
{
    ModelWithLightOutputData op;
    op.svpos = mul(worldMat,pos);
    op.worldPos = op.svpos.xyz;
    op.svpos = mul(viewMat,op.svpos);
    op.svpos = mul(projectionMat,op.svpos);
    op.uv = uv;
    op.normal = normal;

    float4 lightDir = float4(0.0f,-1.0f,0.0f,0.0f);
    lightDir = normalize(lightDir);
    op.lightInTangentWorld = mul(lightDir,InvTangentMatrix(tangent,binormal,normal));

    return op;
}

//乗算ありPS
float4 PSPosNormalUvLightMain(ModelWithLightOutputData input) : SV_TARGET
{
    float4 normalColor = NormalTex.Sample(smp,input.uv);

    float3 bright = float3(0.0f,0.0f,0.0f);
    if(IsEnableToUseMaterialTex(normalColor))
    {
        //-1.0f ~ 1.0f
        float3 normalVec = 2 * normalColor.xyz - 1.0f;
        normalVec = normalize(normalVec);
        bright = dot(input.lightInTangentWorld,normalVec);
    }
    else
    {
        bright = dot(input.lightInTangentWorld,input.normal);
    }

	float4 texColor = AlbedoTex.Sample(smp,input.uv);
    texColor *= colorB1;
	return float4(texColor.rgb * bright, texColor.a);
}


struct PosUvNormalTangentBinormalOutput
{
    float4 svpos : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 worldPos : POSITION;
    float3 tangent : TANGENT2;
    float3 binormal : BINORMAL;
};

//ディファードレンダリング対応
PosUvNormalTangentBinormalOutput VSDefferdMain(float4 pos : POSITION,float3 normal : NORMAL,float2 uv:TEXCOORD,float3 tangent : TANGENT,float3 binormal : BINORMAL)
{
    PosUvNormalTangentBinormalOutput op;
    op.svpos = mul(worldMat,pos);
    op.worldPos = op.svpos.xyz;
    op.svpos = mul(viewMat,op.svpos);
    op.svpos = mul(projectionMat,op.svpos);
    op.uv = uv;
    op.normal = normal;
    op.binormal = binormal;
    op.tangent = tangent;

    return op;
}

struct GBufferOutput
{
    float4 albedo : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 metalnessRoughness : SV_TARGET2;
    float4 world : SV_TARGET3;
};

cbuffer ColorBuffer : register(b2)
{
    float4 color;
}

//ディファードレンダリング対応
GBufferOutput PSDefferdMain(PosUvNormalTangentBinormalOutput input) : SV_TARGET
{
    float4 normalColor = NormalTex.Sample(smp,input.uv);
    //-1.0f ~ 1.0f
    float3 normalVec = 2 * normalColor - 1.0f;
    normalVec = normalize(normalVec);

    float3 normal = mul(rotaion,float4(input.normal,1.0f));
    normal = normalize(normal);
    float3 tangent = mul(rotaion,float4(input.tangent,1.0f));
    tangent = normalize(tangent);
    float3 binormal = cross(normal,tangent);

    float3 nWorld = CalucurateTangentToLocal(normalVec,normal,tangent,binormal);
    if(IsEnableToUseMaterialTex(normalColor))
    {
        nWorld = input.normal;
    }

	float4 texColor = AlbedoTex.Sample(smp,input.uv);
    float4 mrColor = MetalnessRoughnessTex.Sample(smp,input.uv);

    if(IsEnableToUseMaterialTex(mrColor))
    {
        mrColor.xyz = float3(0.0f,0.0f,0.0f);
    }

    GBufferOutput output;
    output.albedo = texColor * color;
    output.normal = float4(nWorld,1.0f);
    output.metalnessRoughness = float4(mrColor.xyz,raytracingId);
    output.world = float4(input.worldPos,1.0f);
	return output;
}
