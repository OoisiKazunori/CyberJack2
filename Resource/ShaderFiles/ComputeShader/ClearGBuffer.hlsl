

RWTexture2D<float4> albedoBuffer : register(u0);

[numthreads(1, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID)
{
    albedoBuffer[groupId.xy] = float4(0,0,0,1);
}