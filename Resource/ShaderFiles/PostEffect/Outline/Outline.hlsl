//���͏��
RWTexture2D<float4> OutlineMap : register(u0);

//�o�͐�UAV  
RWTexture2D<float4> Albedo : register(u1);
RWTexture2D<float4> Emissive : register(u2);

cbuffer Dissolve : register(b0)
{
    float4 m_outlineColor;
}

float4 SamplingPixel(uint2 arg_uv)
{
    return OutlineMap[uint2(clamp(arg_uv.x, 0, 1280), clamp(arg_uv.y, 0, 720))].xyzw;
}

bool OutlineCheck(uint2 arg_dtid, uint2 arg_offset)
{
    float4 pickColor = SamplingPixel(arg_dtid + arg_offset);
    bool isNoise = !(0.9f < pickColor.x && pickColor.y < 0.1f && pickColor.z < 0.1f); //�G�̃m�C�Y�ŏ�������������Ȃ���
    isNoise &= 0.9f <= pickColor.w;
    if (isNoise)
    {
        Albedo[arg_dtid.xy] = pickColor;
        Albedo[arg_dtid.xy].xyz *= Albedo[arg_dtid.xy].w;
        Emissive[arg_dtid.xy] = pickColor;
        Emissive[arg_dtid.xy].xyz *= Emissive[arg_dtid.xy].w;
        return true;
    }
    
    return false;
}

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    
    //�p�e�B�N������������点�Ȃ��B
    //���������Ƃ���ɂ͕`�悵�Ȃ��B
    bool isParticle = OutlineMap[DTid.xy].x <= -0.9f && OutlineMap[DTid.xy].y <= -0.9f && OutlineMap[DTid.xy].z <= -0.9f;
    bool isNonObject = length(OutlineMap[DTid.xy].xyz) <= 0.1f;
    if (isParticle || isNonObject)
    {
        
        Albedo[DTid.xy] = float4(0, 0, 0, 0);
        Emissive[DTid.xy] = float4(0, 0, 0, 0);
        return;
        
    }
    
    //�������G�̃m�C�Y�ŏ������������ǂ����B
    bool isEnmey = 0.9f < SamplingPixel(DTid.xy).x && SamplingPixel(DTid.xy).y < 0.1f && SamplingPixel(DTid.xy).z < 0.1f;
    if (!isEnmey)
    {
        Albedo[DTid.xy] = float4(0, 0, 0, 0);
        Emissive[DTid.xy] = float4(0, 0, 0, 0);
        return;
    }
    
    //�E�����`�F�b�N
    if (OutlineCheck(DTid.xy, uint2(1, 0)))
    {
        return;
    }
    
    //�������`�F�b�N
    if (OutlineCheck(DTid.xy, uint2(-1, 0)))
    {
        return;
    }
    
    //�㑤���`�F�b�N
    if (OutlineCheck(DTid.xy, uint2(0, 1)))
    {
        return;
    }
    
    //�������`�F�b�N
    if (OutlineCheck(DTid.xy, uint2(0, -1)))
    {
        return;
    }
    

        
    Albedo[DTid.xy] = float4(0, 0, 0, 0);
    Emissive[DTid.xy] = float4(0, 0, 0, 0);

}