
//���͏��
RWTexture2D<float4> BackBuffer : register(u0);

//���͏��
RWTexture2D<float4> Raytracing : register(u1);

//�o�͐�UAV  
RWTexture2D<float4> OutputImg : register(u2);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    //�w�i�F�������烌�C�g���̌��ʂ�`��
    bool isBackGroundColor = 0.8f <= BackBuffer[DTid.xy].r && BackBuffer[DTid.xy].g <= 0.01f && BackBuffer[DTid.xy].b <= 0.01f;
    if (isBackGroundColor)
    {
    
        OutputImg[DTid.xy] = Raytracing[DTid.xy];
        
    }
    else
    {
        
        OutputImg[DTid.xy] = BackBuffer[DTid.xy];
    
    }
    
}