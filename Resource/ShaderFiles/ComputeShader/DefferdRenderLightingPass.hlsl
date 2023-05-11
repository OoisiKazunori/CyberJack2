RWTexture2D<float4> finalGBuffer : register(u0);
RWTexture2D<float4> albedoGBuffer : register(u1);
RWTexture2D<float4> normalGBuffer : register(u2);

//���C�e�B���O�p�X---------------------------------------
[numthreads(1, 1, 1)]
void CSLightingPass(uint3 groupId : SV_GroupID)
{    
    finalGBuffer[groupId.xy] = albedoGBuffer[groupId.xy] * normalGBuffer[groupId.xy];
}
//���C�e�B���O�p�X---------------------------------------