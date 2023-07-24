
//���͏��
RWTexture2D<float4> InputImg : register(u0);

//�o�͐�UAV  
RWTexture2D<float4> OutputImg : register(u1);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{

    //�e�N�X�`���̃T�C�Y
    const float2 TEXSIZE = float2(1280.0f, 720.0f);
    
    //�T���v�����O����e�N�X�`���̏㉺���E�𔽓]������B
    float2 texpos = float2(DTid.x, DTid.y) / TEXSIZE;
    texpos = -texpos + float2(1.0f, 1.0f);
    
    //������g���B
    float2 texelSize = 1.0f / TEXSIZE;
 
    //��ʒ��S�Ɍ������Ẵx�N�g�������߂�B�S�[�X�g�ɂ��΂������B
    float ghostDispersal = 0.36f;
    float2 ghostVec = (float2(0.5f, 0.5f) - texpos) * ghostDispersal;
   
    //�S�[�X�g���T���v�����O����B
    float4 sampleGhost = float4(0,0,0,0);
    uint ghostCount = 8;
    for (int i = 0; i < ghostCount; ++i)
    {
        float2 offset = frac(texpos + ghostVec * float(i));
        
        float weight = length(float2(0.5f, 0.5f) - offset) / length(float2(0.5f, 0.5f));
        weight = pow(1.0 - weight, 10.0);
  
        sampleGhost += InputImg[uint2(offset * TEXSIZE)] * weight;
    }
    
    OutputImg[DTid.xy] = sampleGhost;
    
}