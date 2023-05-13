RWTexture2D<float4> finalGBuffer : register(u0);
RWTexture2D<float4> albedoGBuffer : register(u1);
RWTexture2D<float4> normalGBuffer : register(u2);

//ライティングパス---------------------------------------
[numthreads(1, 1, 1)]
void CSLightingPass(uint3 groupId : SV_GroupID)
{
    float3 light = normalize(float3(1, -1, 1));
    float diffuse = saturate(dot(-light, normalGBuffer[groupId.xy].xyz));
    float brightness = diffuse + 0.3f;

    if(normalGBuffer[groupId.xy].w == 0.0f)
    {
        brightness = 1.0f;
    }

    //法線に情報を何も書いていなければライトの影響を受けない
    finalGBuffer[groupId.xy] = albedoGBuffer[groupId.xy] * float4(brightness,brightness,brightness,1);
}
//ライティングパス---------------------------------------