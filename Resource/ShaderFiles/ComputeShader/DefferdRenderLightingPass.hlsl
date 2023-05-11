RWTexture2D<float4> finalGBuffer : register(u0);
RWTexture2D<float4> albedoGBuffer : register(u1);
RWTexture2D<float4> normalGBuffer : register(u2);

//ライティングパス---------------------------------------
[numthreads(1, 1, 1)]
void CSLightingPass(uint3 groupId : SV_GroupID)
{    
    finalGBuffer[groupId.xy] = albedoGBuffer[groupId.xy] * normalGBuffer[groupId.xy];
}
//ライティングパス---------------------------------------