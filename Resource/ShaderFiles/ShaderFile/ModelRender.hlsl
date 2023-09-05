
Texture2D<float4>AlbedoTex:register(t0);
Texture2D<float4>NormalTex:register(t1);
Texture2D<float4>MetalnessRoughnessTex:register(t2);
Texture2D<float4>EmissiveTex:register(t3);
SamplerState smp :register(s0);

cbuffer MaterialID : register(b1)
{
    uint raytracingId;
}

struct GBufferOutput
{
    float4 albedo : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 metalnessRoughness : SV_TARGET2;
    float4 world : SV_TARGET3;
    float4 emissive : SV_TARGET4;
};

//https://coposuke.hateblo.jp/entry/2020/12/21/144327
float3 CalucurateLocalToTangent(float3 localVector,float3 normal,float3 tangent,float3 binNoraml)
{
    float3 tangentVector = float3(dot(tangent,localVector),dot(binNoraml,localVector),dot(normal,localVector));
    tangentVector = normalize(tangentVector);
    return tangentVector;
}

float3 CalucurateTangentToLocal(float3 tangentVector,float3 normal,float3 tangent,float3 binNoraml)
{
    float3 localVector = tangent * tangentVector.x + binNoraml * tangentVector.y + normal * tangentVector.z;
    return localVector;
}

matrix InvTangentMatrix(float3 tangent,float3 binormal,float3 normal)
{
   float4x4 mat =
   {
        float4(tangent, 0.0f),
        float4(binormal,0.0f),
        float4(normal  ,0.0f),
        float4(0,0,0,1)
    };
   return transpose(mat);
}

bool IsEnableToUseMaterialTex(float4 texColor)
{
    return texColor.a <= 0.0f;
};

cbuffer MatBuffer : register(b0)
{
    matrix worldMat;
    matrix viewMat;
    matrix projectionMat;
    matrix rotaion;
}

cbuffer ColorBuffer : register(b1)
{
    float4 colorB1;
}

cbuffer cbuff2 : register(b2)
{
    matrix bonesB2[256];
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

struct PosUvNormalTangentBinormalOutput
{
    float4 svpos : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 worldPos : POSITION;
    float3 tangent : TANGENT2;
    float3 binormal : BINORMAL;
};

//モデルのアニメーション
PosUvNormalTangentBinormalOutput VS(VertexData input)
{
    float4 resultPos = input.pos;
    //static const int NO_BONE = -1;
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
    //    matrix mat = bonesB2[input.boneNo[0]] * input.weight[0];
    //    for (int i = 1; i < num; ++i)
    //    {
    //        mat += bonesB2[input.boneNo[i]] * input.weight[i];
    //    }
    //    resultPos = mul(mat, input.pos);
    //}
    //else if (input.boneNo[1] != NO_BONE)
    //{
    //    matrix mat = bonesB2[input.boneNo[0]] * input.weight[0];
    //    mat += bonesB2[input.boneNo[1]] * (1 - input.weight[0]);
    //    
    //    resultPos = mul(mat, input.pos);
    //}
    //else if (input.boneNo[0] != NO_BONE)
    //{
    //    resultPos = mul(bonesB2[input.boneNo[0]], input.pos);
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

GBufferOutput PS(PosUvNormalTangentBinormalOutput input) : SV_TARGET
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
    output.albedo = texColor * colorB1;
    output.normal = float4(normal, 1.0f);
    output.metalnessRoughness = float4(mrColor.xyz,1);
    output.world = float4(input.worldPos,1.0f);
    output.emissive = EmissiveTex.Sample(smp,input.uv);
    return output;
}