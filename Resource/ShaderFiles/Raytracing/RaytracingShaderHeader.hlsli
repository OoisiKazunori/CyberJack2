
//�~����
static const float PI = 3.141592653589f;

//���_���
struct Vertex
{
    float3 pos;
    float3 normal;
    float2 uv;
    float3 tangent;
    float3 binormal;
};

//�y�C���[�h
struct Payload
{
    float3 m_color; //�F���
    uint m_rayID;   //���C��ID
};

struct MyAttribute
{
    float2 barys;
};

//���C�g�֌W�̃f�[�^
struct DirLight
{
    float3 m_dir;
    int m_isActive;
};
struct PointLight
{
    float3 m_pos;
    float m_power;
    int3 m_pad;
    int m_isActive;
};
struct LightData
{
    DirLight m_dirLight;
    PointLight m_pointLight;
};

//�J�����p�萔�o�b�t�@
struct CameraEyePosConstData
{
    float3 m_eye;
};

//barys���v�Z
inline float3 CalcBarycentrics(float2 Barys)
{
    return float3(1.0 - Barys.x - Barys.y, Barys.x, Barys.y);
}

//���������ʒu�̏����擾����֐�
Vertex GetHitVertex(MyAttribute attrib, StructuredBuffer<Vertex> vertexBuffer, StructuredBuffer<uint> indexBuffer)
{
    Vertex v = (Vertex) 0;
    float3 barycentrics = CalcBarycentrics(attrib.barys);
    uint vertexId = PrimitiveIndex() * 3; //Triangle List �̂���.

    float weights[3] =
    {
        barycentrics.x, barycentrics.y, barycentrics.z
    };

    for (int index = 0; index < 3; ++index)
    {
        uint vtxIndex = indexBuffer[vertexId + index];
        float w = weights[index];
        v.pos += vertexBuffer[vtxIndex].pos * w;
        v.normal += vertexBuffer[vtxIndex].normal * w;
        v.uv += vertexBuffer[vtxIndex].uv * w;
    }

    return v;
}

//���C�g�����ōs�����C�e�B���O�p�X
void LightingPass(inout float arg_bright, float4 arg_worldPosMap, float4 arg_normalMap, LightData arg_lightData, RaytracingAccelerationStructure arg_scene)
{
    
    //�f�B���N�V�������C�g�B
    if (arg_lightData.m_dirLight.m_isActive && 0.1f < length(arg_normalMap.xyz))    //�u�f�B���N�V�������C�g���L����������v ���� �u���݂̃X�N���[�����W�̈ʒu�ɖ@�����������܂�Ă�����(�����������܂�Ă��Ȃ��Ƃ��납��͉e�p�̃��C���΂��Ȃ��悤�ɂ��邽�߁B)�v
    {
        
        //�y�C���[�h(�ċA�I�ɏ��������郌�C�g���̒��Œl�̎󂯓n���Ɏg�p����\����)��錾�B
        Payload payloadData;
        payloadData.m_color = float3(0.0f, 0.0f, 0.0f); //�F��^�����ɂ��Ă����B���C���΂��Ăǂ��ɂ�������Ȃ��������ɌĂ΂��MissShader���Ă΂ꂽ�炻����1���������ށB
        
        //���C�̐ݒ�
        RayDesc rayDesc;
        rayDesc.Origin = arg_worldPosMap.xyz; //���C�̔��˒n�_��ݒ�B

        rayDesc.Direction = -arg_lightData.m_dirLight.m_dir; //���C�͌����Ɍ������Ĕ�΂��B
        rayDesc.TMin = 1.0f; //���C�̍ŏ��l
        rayDesc.TMax = 300000.0f; //���C�̍ő�l(�J������Far�݂����Ȋ����B)
    
        RAY_FLAG flag = RAY_FLAG_NONE; //���C�̃t���O�B�w�ʃJ�����O��������AAnyHitShader���Ă΂Ȃ��悤�ɂ���(�y�ʉ�)����Ƃ��͂�����ݒ肷��B
        flag |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    
        //���C�𔭎�
        TraceRay(
        arg_scene, //TLAS
        flag,
        0xFF,
        0, //�Œ�ł悵�B
        1, //�Œ�ł悵�B
        1, //MissShader�̃C���f�b�N�X�BRenderScene.cpp��m_pipelineShaders��MissShader��o�^����ۂ�2�Ԗڂɉe�p��MissShader��ݒ肵�Ă���̂ŁA1�ɂ���Ɖe�p���Ă΂��B
        rayDesc,
        payloadData);
        
        //���C�g���̌��ʂ̉e�����������ށB
        arg_bright += payloadData.m_color.x;
        
    }
    
    //�|�C���g���C�g
    if (arg_lightData.m_pointLight.m_isActive && 0.1f < length(arg_normalMap.xyz))    //�u�|�C���g���C�g���L����������v ���� �u���݂̃X�N���[�����W�̈ʒu�ɖ@�����������܂�Ă�����(�����������܂�Ă��Ȃ��Ƃ��납��͉e�p�̃��C���΂��Ȃ��悤�ɂ��邽�߁B)�v
    {
        
        //�y�C���[�h(�ċA�I�ɏ��������郌�C�g���̒��Œl�̎󂯓n���Ɏg�p����\����)��錾�B
        Payload payloadData;
        payloadData.m_color = float3(0.0f, 0.0f, 0.0f); //�F��^�����ɂ��Ă����B���C���΂��Ăǂ��ɂ�������Ȃ��������ɌĂ΂��MissShader���Ă΂ꂽ�炻����1���������ށB
        
        //���C�̐ݒ�
        RayDesc rayDesc;
        rayDesc.Origin = arg_worldPosMap.xyz; //���C�̔��˒n�_��ݒ�B
        
        //�|�C���g���C�g����̃x�N�g�������߂�B
        float3 lightDir = normalize(arg_lightData.m_pointLight.m_pos - rayDesc.Origin);
        float distance = length(arg_lightData.m_pointLight.m_pos - rayDesc.Origin);
        
        //���������C�g�̍ő�e���͈͂��傫�������烌�C���΂��Ȃ��B
        if (distance < arg_lightData.m_pointLight.m_power)
        {

            rayDesc.Direction = lightDir; //���C�͌����Ɍ������Ĕ�΂��B
            rayDesc.TMin = 1.0f; //���C�̍ŏ��l
            rayDesc.TMax = distance; //���C�̍ő�l(�J������Far�݂����Ȋ����B)
    
            RAY_FLAG flag = RAY_FLAG_NONE; //���C�̃t���O�B�w�ʃJ�����O��������AAnyHitShader���Ă΂Ȃ��悤�ɂ���(�y�ʉ�)����Ƃ��͂�����ݒ肷��B
            flag |= RAY_FLAG_SKIP_CLOSEST_HIT_SHADER;
    
            //���C�𔭎�
            TraceRay(
            arg_scene, //TLAS
            flag,
            0xFF,
            0, //�Œ�ł悵�B
            1, //�Œ�ł悵�B
            1, //MissShader�̃C���f�b�N�X�BRenderScene.cpp��m_pipelineShaders��MissShader��o�^����ۂ�2�Ԗڂɉe�p��MissShader��ݒ肵�Ă���̂ŁA1�ɂ���Ɖe�p���Ă΂��B
            rayDesc,
            payloadData);
            
            //�e���Ղ��Ă��Ȃ������疾�邳������������B
            if (0 < payloadData.m_color.x)
            {
                
                //-------------------------------------------------------------------------------�����ɃW���b�N����̃��C�g�̏����������B
            
                //���C�g���邳�̊��������߂�B
                float brightRate = saturate(distance / arg_lightData.m_pointLight.m_power);
        
                //���Ŗ��邳�ɃC�[�W���O��������B
                payloadData.m_color.x = 1.0f - (brightRate * brightRate * brightRate);
                
            }
            
            
        
            //���C�g���̌��ʂ̉e�����������ށB
            arg_bright += payloadData.m_color.x;
            
        }
        
    }

}

void SecondaryPass(float4 arg_materialInfo, float4 arg_normalColor, float4 arg_albedoColor, RaytracingAccelerationStructure arg_scene, CameraEyePosConstData arg_cameraEyePos, inout float4 arg_finalColor)
{
        
    //���C��ID���݂āA���C��ł��ǂ����𔻒f
    if (arg_materialInfo.w != 0 && 0.1f < length(arg_normalColor.xyz))
    {

        //���C�̐ݒ�
        RayDesc rayDesc;
        rayDesc.Origin = arg_normalColor.xyz + arg_normalColor.xyz * 3.0f;

        rayDesc.Direction = refract(normalize(rayDesc.Origin - arg_cameraEyePos.m_eye), arg_normalColor.xyz, 0.01f);
        rayDesc.TMin = 0.0f;
        rayDesc.TMax = 300000.0f;
        
        Payload payloadData;
        payloadData.m_color = float3(1, 1, 1);
    
        RAY_FLAG flag = RAY_FLAG_NONE;
        flag |= RAY_FLAG_CULL_BACK_FACING_TRIANGLES; //�w�ʃJ�����O
    
        //���C�𔭎�
        TraceRay(
        arg_scene, //TLAS
        flag, //�Փ˔��萧�������t���O
        0xFF, //�Փ˔���Ώۂ̃}�X�N�l
        0, //ray index
        1, //MultiplierForGeometryContrib
        0, //miss index
        rayDesc,
        payloadData);
        

        //���ʊi�[
        arg_finalColor = float4(arg_albedoColor.xyz, 1) * arg_materialInfo.y;
        arg_finalColor += float4((payloadData.m_color), 1) * (1.0f - arg_materialInfo.y);
        
    }
    else
    {
        arg_finalColor = arg_albedoColor;
    }
    
}