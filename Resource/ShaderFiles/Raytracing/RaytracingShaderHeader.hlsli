#include "RaytracingNoiseHeader.hlsli"

//�~����
static const float PI = 3.141592653589f;

//�~�X�V�F�[�_�[�̃C���f�b�N�X
static const int MISS_DEFAULT = 0;
static const int MISS_LIGHTING = 1;

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
    float m_timer;
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

//���C��������
void CastRay(inout Payload arg_payload, float3 arg_origin, float3 arg_dir, float arg_far, int arg_msIndex, RAY_FLAG arg_rayFlag, RaytracingAccelerationStructure arg_scene)
{
    //���C�̐ݒ�
    RayDesc rayDesc;
    rayDesc.Origin = arg_origin; //���C�̔��˒n�_��ݒ�B

    rayDesc.Direction = arg_dir; //���C�̎ˏo������ݒ�B
    rayDesc.TMin = 1.0f; //���C�̍ŏ��l
    rayDesc.TMax = arg_far; //���C�̍ő�l(�J������Far�݂����Ȋ����B)
    
    //���C�𔭎�
    TraceRay(
        arg_scene, //TLAS
        arg_rayFlag,
        0xFF,
        0, //�Œ�ł悵�B
        1, //�Œ�ł悵�B
        arg_msIndex, //MissShader�̃C���f�b�N�X�BRenderScene.cpp��m_pipelineShaders��MissShader��o�^���Ă���B
        rayDesc,
        arg_payload);
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
        
        //���C������
        CastRay(payloadData, arg_worldPosMap.xyz, -arg_lightData.m_dirLight.m_dir, 30000.0f, MISS_LIGHTING, RAY_FLAG_SKIP_CLOSEST_HIT_SHADER, arg_scene);
        
        //���C�g���̌��ʂ̉e�����������ށB
        arg_bright += payloadData.m_color.x;
        
    }
    
    //�|�C���g���C�g
    if (arg_lightData.m_pointLight.m_isActive && 0.1f < length(arg_normalMap.xyz))    //�u�|�C���g���C�g���L����������v ���� �u���݂̃X�N���[�����W�̈ʒu�ɖ@�����������܂�Ă�����(�����������܂�Ă��Ȃ��Ƃ��납��͉e�p�̃��C���΂��Ȃ��悤�ɂ��邽�߁B)�v
    {
        
        //�y�C���[�h(�ċA�I�ɏ��������郌�C�g���̒��Œl�̎󂯓n���Ɏg�p����\����)��錾�B
        Payload payloadData;
        payloadData.m_color = float3(0.0f, 0.0f, 0.0f); //�F��^�����ɂ��Ă����B���C���΂��Ăǂ��ɂ�������Ȃ��������ɌĂ΂��MissShader���Ă΂ꂽ�炻����1���������ށB
        
        //�|�C���g���C�g����̃x�N�g�������߂�B
        float3 lightDir = normalize(arg_lightData.m_pointLight.m_pos - arg_worldPosMap.xyz);
        float distance = length(arg_lightData.m_pointLight.m_pos - arg_worldPosMap.xyz);
        
        //���������C�g�̍ő�e���͈͂��傫�������烌�C���΂��Ȃ��B
        if (distance < arg_lightData.m_pointLight.m_power)
        {
            
        
            //���C������
            CastRay(payloadData, arg_worldPosMap.xyz, lightDir, distance, MISS_LIGHTING, RAY_FLAG_SKIP_CLOSEST_HIT_SHADER, arg_scene);
            
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


void GodRayPass(float4 arg_worldColor, inout float4 arg_albedoColor, uint2 arg_launchIndex, CameraEyePosConstData arg_cameraEyePos, LightData arg_lightData, RaytracingAccelerationStructure arg_scene)
{
    
    //�J��������T���v�����O�n�_�܂ł����C�}�[�`���O�œ������萔�Ŋ���A�T���v�����O�񐔂����߂�B
    const int SAMPLING_COUNT = 16;
    float3 samplingDir = normalize(arg_worldColor.xyz - arg_cameraEyePos.m_eye);
    float samplingLength = length(arg_cameraEyePos.m_eye - arg_worldColor.xyz) / (float) SAMPLING_COUNT;
    float raymarchingBright = 0.0f;
    float fogValue = 0.0f;
    for (int counter = 0; counter < SAMPLING_COUNT; ++counter)
    {
                
        //�y�C���[�h(�ċA�I�ɏ��������郌�C�g���̒��Œl�̎󂯓n���Ɏg�p����\����)��錾�B
        Payload payloadData;
        payloadData.m_color = float3(0.0f, 0.0f, 0.0f); //�F��^�����ɂ��Ă����B���C���΂��Ăǂ��ɂ�������Ȃ��������ɌĂ΂��MissShader���Ă΂ꂽ�炻����1���������ށB�����ɓ��������Ƃ��ɌĂ΂��ClosestHitShader���Ă΂ꂽ�炻���͉e�Ȃ̂�0���������ށB
        
        //����
        float progress = 1.0f / float(counter + 1.0f);
        
        //�f�B���N�V�������C�g�̏���
        if (arg_lightData.m_dirLight.m_isActive)
        {
        
            //���C������
            CastRay(payloadData, arg_cameraEyePos.m_eye + (samplingDir * samplingLength) * counter, -arg_lightData.m_dirLight.m_dir, 300000.0f, MISS_LIGHTING, RAY_FLAG_SKIP_CLOSEST_HIT_SHADER, arg_scene);
        
            //���ʂ�ۑ��B
            raymarchingBright = lerp(raymarchingBright, payloadData.m_color.x, progress);
            
        }
        //�|�C���g���C�g�̏���
        float pointLightDistance = length((arg_cameraEyePos.m_eye + (samplingDir * samplingLength) * counter) - arg_worldColor.xyz);
        if (arg_lightData.m_pointLight.m_isActive && pointLightDistance < arg_lightData.m_pointLight.m_power)
        {
        
            //���C������
            CastRay(payloadData, arg_cameraEyePos.m_eye + (samplingDir * samplingLength) * counter, -arg_lightData.m_dirLight.m_dir, pointLightDistance, MISS_LIGHTING, RAY_FLAG_SKIP_CLOSEST_HIT_SHADER, arg_scene);
        
            //���ʂ�ۑ��B
            raymarchingBright = lerp(raymarchingBright, payloadData.m_color.x, progress);
            
        }
        
        //�t�H�O�����߂�B
        float3 worldPos = arg_cameraEyePos.m_eye + (samplingDir * samplingLength) * counter;
        float3 st = float3(arg_launchIndex, (float)((int) worldPos.z % 256) / 256.0f);
        st.x /= 1280.0f;
        st.y /= 720.0f;
        float3 noiseColor = PerlinNoiseWithWind(st * 10.0f, 7, 0.73f, 2.3f, 0.1f, 7.0f, arg_cameraEyePos.m_timer, arg_cameraEyePos.m_eye, 0.0f);
        float3 weights = float3(0.8f, 0.1f, 0.1f); // �e�m�C�Y�̏d��
        fogValue = lerp(fogValue, dot(noiseColor, weights), progress);
        
    }
    const float3 FOGCOLOR_LIT = float3(1.0f, 1.0f, 1.0f);
    const float3 FOGCOLOR_UNLIT = float3(0.0f, 0.0f, 0.0f);
    
    float3 fogColor = lerp(FOGCOLOR_UNLIT, FOGCOLOR_LIT, raymarchingBright);
    const float FOG_DENSITY = 0.0001f;
    float absorb = exp(-length(arg_cameraEyePos.m_eye - arg_worldColor.xyz) * FOG_DENSITY);
    arg_albedoColor.xyz = lerp(fogColor, arg_albedoColor.xyz, absorb);
    arg_albedoColor.xyz += float3(fogValue, fogValue, fogValue);
    
}

void SecondaryPass(float4 arg_worldColor, float4 arg_materialInfo, float4 arg_normalColor, float4 arg_albedoColor, RaytracingAccelerationStructure arg_scene, CameraEyePosConstData arg_cameraEyePos, inout float4 arg_finalColor)
{
        
    //���C��ID���݂āA���C��ł��ǂ����𔻒f
    if (arg_materialInfo.w != 0 && 0.1f < length(arg_normalColor.xyz))
    {
        
        Payload payloadData;
        payloadData.m_color = float3(1, 1, 1);
        
        //���C������
        float rayOrigin = arg_worldColor.xyz + arg_normalColor.xyz * 3.0f;
        CastRay(payloadData, rayOrigin, refract(normalize(rayOrigin - arg_cameraEyePos.m_eye), arg_normalColor.xyz, 0.01f), 300000.0f, MISS_DEFAULT, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, arg_scene);
        

        //���ʊi�[
        arg_finalColor = float4(arg_albedoColor.xyz, 1) * arg_materialInfo.y;
        arg_finalColor += float4((payloadData.m_color), 1) * (1.0f - arg_materialInfo.y);
        
    }
    else
    {
        arg_finalColor = arg_albedoColor;
    }
    
}