#include"ModelBuffer.hlsli"

cbuffer MatBuffer : register(b0)
{
    matrix mat; //3D変換行列
}

cbuffer MatBuffer : register(b1)
{
    float3 localLightDir;
}

//uv、法線対応---------------------------------------
struct PosUvNormalOutput
{
    float4 svpos : SV_POSITION; //システム用頂点座標
    float3 normal : NORMAL; //法線ベクトル
    float2 uv : TEXCOORD;
    float3 lightInTangentWorld : TANGENT;
};

PosUvNormalOutput VSPosNormalUvmain(float4 pos : POSITION,float3 normal : NORMAL,float2 uv:TEXCOORD,float3 tangent : TANGENT,float3 binormal : BINORMAL)
{
    PosUvNormalOutput op;
    op.svpos = mul(mat,pos);
    op.uv = uv;
    op.normal = normal;

    float4 lightDir = float4(localLightDir.xyz,0.0f);
    lightDir = normalize(lightDir);
    //ローカル空間にあるライトをタンジェント空間に移動させる
    op.lightInTangentWorld = mul(lightDir,InvTangentMatrix(tangent,binormal,normal));

    return op;
}

float4 PSPosNormalUvmain(PosUvNormalOutput input) : SV_TARGET
{
    float3 normalColor = NormalTex.Sample(smp,input.uv);
    //-表現を入れる為に、0~255の半分を0.0f地点にするよう計算する。
    //-1.0f ~ 1.0f
    float3 normalVec = 2 * normalColor - 1.0f;
    normalVec = normalize(normalVec);

    //タンジェント空間のベクトルを入れる
    float3 bright = dot(input.lightInTangentWorld,normalVec);
    //bright = max(0.0f,bright);

	float4 texColor = AlbedoTex.Sample(smp,input.uv);
	return float4(texColor.rgb * bright, 1.0f);
}
//uv、法線対応---------------------------------------


//モデル、G-Buffer格納---------------------------------------

PosUvNormalOutput VSDefferdMain(float4 pos : POSITION,float3 normal : NORMAL,float2 uv:TEXCOORD,float3 tangent : TANGENT,float3 binormal : BINORMAL)
{
    PosUvNormalOutput op;
    op.svpos = mul(mat,pos);
    op.uv = uv;
    op.normal = normal;

    float4 lightDir = float4(localLightDir.xyz,0.0f);
    lightDir = normalize(lightDir);
    //ローカル空間にあるライトをタンジェント空間に移動させる
    op.lightInTangentWorld = mul(lightDir,InvTangentMatrix(tangent,binormal,normal));

    return op;
}

struct GBufferOutput
{
    float4 albedo : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 metalnessRoughness : SV_TARGET2;
};

GBufferOutput PSDefferdMain(PosUvNormalOutput input) : SV_TARGET
{
    float3 normalColor = NormalTex.Sample(smp,input.uv);
    //-表現を入れる為に、0~255の半分を0.0f地点にするよう計算する。
    //-1.0f ~ 1.0f
    float3 normalVec = 2 * normalColor - 1.0f;
    normalVec = normalize(normalVec);

    //タンジェント空間のベクトルを入れる
    float3 bright = dot(input.lightInTangentWorld,normalVec);

	float4 texColor = AlbedoTex.Sample(smp,input.uv);

    float4 mrColor = MetalnessRoughnessTex.Sample(smp,input.uv);

    GBufferOutput output;
    output.albedo = texColor;
    output.normal.xyz = normalColor;
    output.metalnessRoughness = mrColor;
    output.normal.a = 1.0f;
	return output;
}

//モデル、G-Buffer格納---------------------------------------