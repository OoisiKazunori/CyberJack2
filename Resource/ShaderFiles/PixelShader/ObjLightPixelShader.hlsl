#include"../ShaderHeader/OBJShaderHeader.hlsli"
#include"../ShaderHeader/BaseShaderHeader.hlsli"

cbuffer cbuff2 : register(b2)
{
    float3 lightV;
    float3 blightColor;
    float3 eyePos;
};

Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

float4 PSmain(VSLightOutput input) : SV_TARGET
{
    //ライトの色
    //float3 lightColor = blightColor;    
    float4 texColor = tex.Sample(smp, input.uv);
    
    float3 ambientData = m_ambient;
    float3 diffuseData = m_diffuse;
    float3 specularData = m_specular;
    

    //光沢度
    const float shininess = 4.0f;
    //頂点から視点への方向ベクトル
    float3 eyeDir = normalize(eyePos - input.worldPos.xyz);
    //ライトに向かうベクトルと法線の内積
    float3 dotLightNormal = dot(lightV, input.normal);
    //反射光ベクトル
    float3 reflect = normalize(-lightV + 2 * dotLightNormal * input.normal);
    
    
    //環境反射光
    float3 ambient = ambientData;
    //拡散反射光
    float3 diffuse = dotLightNormal * diffuseData;
    
    
    //鏡面反射光    
    float3 resultDot = dot(reflect, eyeDir);
    float3 resultSaturate = saturate(resultDot);    
    float3 specular = pow(resultSaturate, shininess) * specularData;

    //合成
    float4 shaderColor;
    shaderColor.rgb = (ambient + diffuse + specular);
    shaderColor.a = m_alpha;

    return float4(shaderColor * texColor);
}