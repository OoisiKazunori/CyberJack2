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

cbuffer ColorBuffer : register(b2)
{
    float4 colorB2;
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

    float4 lightDir = float4(0.0f,1.0f,0.0f,0.0f);
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
    float ambient = 0.5f;
    bright = clamp(bright,ambient,1.0f);

	float4 texColor = AlbedoTex.Sample(smp,input.uv);
    texColor *= colorB1;
	return float4(texColor.rgb * bright, texColor.a);
}

ModelWithLightOutputData VSPosNormalUvLightmain(float4 pos : POSITION,float3 normal : NORMAL,float2 uv:TEXCOORD,float3 tangent : TANGENT,float3 binormal : BINORMAL)
{
    ModelWithLightOutputData op;
    op.svpos = mul(worldMat,pos);
    op.worldPos = op.svpos.xyz;
    op.svpos = mul(viewMat,op.svpos);
    op.svpos = mul(projectionMat,op.svpos);
    op.uv = uv;
    op.normal = normal;

    float4 lightDir = float4(localLightDir.x,localLightDir.y,localLightDir.z,0.0f);
    lightDir = normalize(lightDir);
    op.lightInTangentWorld = mul(lightDir,InvTangentMatrix(tangent,binormal,normal));

    return op;
}

float4 PSPosNormalUvLightmain(ModelWithLightOutputData input) : SV_TARGET
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
    float ambient = 0.5f;
    bright = clamp(bright,ambient,1.0f);

    float4 texColor = AlbedoTex.Sample(smp,input.uv);
    if(IsAll0MaterialTex(texColor))
    {
        texColor = colorB2;
    }
    else
    {
        texColor *= colorB2;
    }
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
    output.normal = float4(normal, 1.0f);
    output.metalnessRoughness = float4(mrColor.xyz,raytracingId);
    output.world = float4(input.worldPos,1.0f);
    output.emissive = float4(0,0,0,1);
	return output;
}

cbuffer EmissiveBuffer : register(b3)
{
    float4 emissiveColorAndStrength;
}

//ディファードレンダリング対応、ブルーム付き
GBufferOutput PSDefferdBloomMain(PosUvNormalTangentBinormalOutput input) : SV_TARGET
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
    output.normal = float4(normal, 1.0f);
    output.metalnessRoughness = float4(mrColor.xyz,raytracingId);
    output.world = float4(input.worldPos,1.0f);
    float3 emissiveColor = emissiveColorAndStrength.xyz * emissiveColorAndStrength.a;
    output.emissive = float4(emissiveColor,1.0f);
	return output;
}

struct VertexData
{
    float4 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    int4 boneNo : BONE_NO;
    float4 weight : WEIGHT;
};
static const int NO_BONE = -1;
//骨
cbuffer cbuff4 : register(b3)
{
    matrix bones[256];
}

//モデルのアニメーション
PosUvNormalTangentBinormalOutput VSDefferdAnimationMain(VertexData input)
{
    float4 resultPos = input.pos;
    //if (input.boneNo[2] != NO_BONE)
    //{
    //    int num;
    //    
    //    if (input.boneNo[3] != NO_BONE)
    //    {
    //        num = 4;
    //    }
    //    else
    //    {
    //        num = 3;
    //    }
    //    
    //    matrix mat = bones[input.boneNo[0]] * input.weight[0];
    //    for (int i = 1; i < num; ++i)
    //    {
    //        mat += bones[input.boneNo[i]] * input.weight[i];
    //    }
    //    resultPos = mul(mat, input.pos);
    //}
    //else if (input.boneNo[1] != NO_BONE)
    //{
    //    matrix mat = bones[input.boneNo[0]] * input.weight[0];
    //    mat += bones[input.boneNo[1]] * (1 - input.weight[0]);
    //    
    //    resultPos = mul(mat, input.pos);
    //}
    //else if (input.boneNo[0] != NO_BONE)
    //{
    //    resultPos = mul(bones[input.boneNo[0]], input.pos);
    //}

    PosUvNormalTangentBinormalOutput op;
    op.svpos = mul(worldMat,resultPos);
    op.worldPos = op.svpos.xyz;
    op.svpos = mul(viewMat,op.svpos);
    op.svpos = mul(projectionMat,op.svpos);
    op.uv = input.uv;
    op.normal = input.normal;
    op.binormal = input.binormal;
    op.tangent = input.tangent;

    return op;
}

//ディファードレンダリング対応
float4 PSDefferdAnimationMain(PosUvNormalTangentBinormalOutput input) : SV_TARGET
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
    output.normal = float4(normal, 1.0f);
    output.metalnessRoughness = float4(mrColor.xyz,raytracingId);
    output.world = float4(input.worldPos,1.0f);
    output.emissive = float4(0,0,0,1);
	return texColor * color;
}