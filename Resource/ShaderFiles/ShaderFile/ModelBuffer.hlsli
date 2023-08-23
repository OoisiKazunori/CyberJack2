
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