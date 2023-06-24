//�A���x�h�p�̃o�b�t�@
Texture2D<float4>AlbedoTex:register(t0);
//�@���}�b�v�p�̃o�b�t�@
Texture2D<float4>NormalTex:register(t1);
//���t�l�X�A���^���l�X�p�̃o�b�t�@
Texture2D<float4>MetalnessRoughnessTex:register(t2);

SamplerState smp :register(s0);

cbuffer MaterialID : register(b1)
{
    //0...�������Ȃ��A1...���˂���A2...���܂���
    uint raytracingId;
}

//�ϊ��s��̌v�Z�̎Q�l�T�C�g
//https://coposuke.hateblo.jp/entry/2020/12/21/144327

//���[�J����� �� �ڋ�� �̕ϊ��s��
float3 CalucurateLocalToTangent(float3 localVector,float3 normal,float3 tangent,float3 binNoraml)
{
    float3 tangentVector = float3(dot(tangent,localVector),dot(binNoraml,localVector),dot(normal,localVector));
    return tangentVector;
}

//�ڋ�� �� ���[�J����Ԃ̕ϊ��s��
float3 CalucurateTangentToLocal(float3 tangentVector,float3 normal,float3 tangent,float3 binNoraml)
{
    float3 localVector = tangent * tangentVector.x + binNoraml * tangentVector.y + normal * tangentVector.z;
    return localVector;
}

//�ڋ�Ԃ̋t�s��
matrix InvTangentMatrix(float3 tangent,float3 binormal,float3 normal)
{
   float4x4 mat =
   {
        float4(tangent, 0.0f),
        float4(binormal,0.0f),
        float4(normal  ,0.0f),
        float4(0,0,0,1)
    };
    //���K�����n�Ȃ̂œ]�u����΋t�s��ɂȂ�
   return transpose(mat);
}

bool IsEnableToUseMaterialTex(float4 texColor)
{
    return texColor.a <= 0.0f;
};