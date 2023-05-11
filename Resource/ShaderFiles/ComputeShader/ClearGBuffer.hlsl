RWTexture2D<float4> albedoGBuffer : register(u0);
RWTexture2D<float4> normalGBuffer : register(u1);

//クリア処理---------------------------------------
[numthreads(1, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID)
{    
    albedoGBuffer[groupId.xy] = float4(0,0,0,1);
    normalGBuffer[groupId.xy] = float4(0,0,0,1);
}
//クリア処理---------------------------------------

