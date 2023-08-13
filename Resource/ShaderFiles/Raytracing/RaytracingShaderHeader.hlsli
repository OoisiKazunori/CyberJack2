#include "RaytracingNoiseHeader.hlsli"

//�~����
static const float PI = 3.141592653589f;

//�~�X�V�F�[�_�[�̃C���f�b�N�X
static const int MISS_DEFAULT = 0;
static const int MISS_LIGHTING = 1;
static const int MISS_CHECKHIT = 2;

//�}�e���A���̎��
static const int MATERIAL_NONE = 0;
static const int MATERIAL_REFLECT = 1;
static const int MATERIAL_REFRACT = 2;
static const int MATERIAL_SEA = 3;

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
    uint m_rayID; //���C��ID
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
    matrix m_viewMat;
    matrix m_projMat;
    float3 m_eye;
    float m_timer;
};

//�{�����[���t�H�O�p�萔�o�b�t�@
struct RaymarchingParam
{
    float3 m_pos; //�{�����[���e�N�X�`���̃T�C�Y
    float m_gridSize; //�T���v�����O����O���b�h�̃T�C�Y
    float3 m_color; //�t�H�O�̐F
    float m_wrapCount; //�T���v�����O���W���͂ݏo�����ۂɉ���܂�Wrap���邩
    float m_sampleLength; //�T���v�����O����
    float m_density; //�Z�x�W��
    int m_isSimpleFog;
    int m_isActive;
};

//OnOff�f�o�b�O
struct DebugOnOffParam
{
    int m_debugReflection;
    int m_debugShadow;
    float m_sliderRate;
    float m_pad;
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
void LightingPass(inout float arg_bright, float4 arg_worldPosMap, float4 arg_normalMap, LightData arg_lightData, uint2 arg_launchIndex, DebugOnOffParam arg_debugOnOffParam, RaytracingAccelerationStructure arg_scene)
{
    
    //�f�B���N�V�������C�g�B
    if (arg_lightData.m_dirLight.m_isActive && 0.1f < length(arg_normalMap.xyz))    //�u�f�B���N�V�������C�g���L����������v ���� �u���݂̃X�N���[�����W�̈ʒu�ɖ@�����������܂�Ă�����(�����������܂�Ă��Ȃ��Ƃ��납��͉e�p�̃��C���΂��Ȃ��悤�ɂ��邽�߁B)�v
    {
        
        //�y�C���[�h(�ċA�I�ɏ��������郌�C�g���̒��Œl�̎󂯓n���Ɏg�p����\����)��錾�B
        Payload payloadData;
        payloadData.m_color = float3(0.0f, 0.0f, 0.0f); //�F��^�����ɂ��Ă����B���C���΂��Ăǂ��ɂ�������Ȃ��������ɌĂ΂��MissShader���Ă΂ꂽ�炻����1���������ށB
        
        //���C������
        if (arg_debugOnOffParam.m_debugShadow == 1 && arg_launchIndex.x < arg_debugOnOffParam.m_sliderRate)
        {
            payloadData.m_color = float3(1, 1, 1);
        }
        else
        {
            CastRay(payloadData, arg_worldPosMap.xyz, -arg_lightData.m_dirLight.m_dir, 30000.0f, MISS_LIGHTING, RAY_FLAG_SKIP_CLOSEST_HIT_SHADER, arg_scene);
        }
        
        //���C�g�̃x�N�g���Ɩ@�����疾�邳���v�Z����B
        float bright = saturate(dot(arg_normalMap.xyz, -arg_lightData.m_dirLight.m_dir));
        
        //���C�g���̌��ʂ̉e�����������ށB
        arg_bright += payloadData.m_color.x * bright;
        
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



//��̐F���擾�B
float3 GetSkyColor(float3 arg_eyeVec)
{
    arg_eyeVec.y = max(arg_eyeVec.y, 0.0f);
    float r = pow(1.0f - arg_eyeVec.y, 2.0f);
    float g = 1.0f - arg_eyeVec.y;
    float b = 0.6f + (1.0f - arg_eyeVec.y) * 0.4f;
    return float3(r, g, b);
}

//���C�e�B���O�Ɋւ���֐�
float Diffuse(float3 arg_normal, float3 arg_light, float arg_position)
{
    return pow(dot(arg_normal, arg_light) * 0.4f + 0.6f, arg_position);
}
float Specular(float3 arg_normal, float3 arg_light, float3 arg_eye, float arg_specular)
{
    float nrm = (arg_specular + 8.0f) / (PI * 8.0f);
    return pow(max(dot(reflect(arg_eye, arg_normal), arg_light), 0.0f), arg_specular) * nrm;
}

//�C�̐F���擾
static const float3 SEA_BASE = float3(0.1f, 0.19f, 0.22f); //��B �C�����������瓮�����Ă݂ĉ����𔻒f����B
static const float3 SEA_WATER_COLOR = float3(0.8f, 0.9f, 0.6f); //���O�I�ɐ��̐F
float3 GetSeaColor(float3 arg_position, float3 arg_normal, float3 arg_light, float3 arg_rayDir, float3 arg_dist /*arg_position - ���C�̌��_*/)
{
    //�C�Ɋւ���萔 �����o�����炱����萔�o�b�t�@�ɓ���ĕς�����悤�ɂ���B
    const float SEA_HEIGHT = 0.6f; //�C�̌��E�̍����H
    
    //�t���l���̌v�Z�Ŕ��˗������߂�B http://marupeke296.com/DXPS_PS_No7_FresnelReflection.html
    float fresnel = clamp(1.0f - dot(arg_normal, -arg_rayDir), 0.0f, 1.0f);
    fresnel = pow(fresnel, 3.0f) * 0.65f;

    //���ˁA���܂����ꍇ�̐F�����߂�B
    float3 reflected = GetSkyColor(reflect(arg_rayDir, arg_normal));
    float3 refracted = SEA_BASE + Diffuse(arg_normal, arg_light, 80.0f) * SEA_WATER_COLOR * 0.12f; //�C�̐F ���̐�ɃI�u�W�F�N�g������ꍇ�A���̋����ɉ����Ă��̐F���Ԃ���B

    //�t���l���̌v�Z�œ���ꂽ���˗�����F���Ԃ���B
    float3 color = lerp(refracted, reflected, fresnel);

    //�����������߂�B
    float atten = max(1.0f - dot(arg_dist, arg_dist) * 0.001f, 0.0f);
    color += SEA_WATER_COLOR * (arg_position.y - SEA_HEIGHT) * 0.18f * atten; //�g�̍����ɂ���ĐF��ς��Ă�H�����𒲐�����Δ����ł��邩���H

    //�X�y�L���������߂Č�����o���I
    color += float3(float3(1.0f, 1.0f, 1.0f) * Specular(arg_normal, arg_light, arg_rayDir, 60.0f));

    return color;
}

//�S���̗v�f������̒l�ȓ��Ɏ��܂��Ă��邩�B
bool IsInRange(float3 arg_value, float arg_range, float arg_wrapCount)
{
    
    bool isInRange = arg_value.x / arg_range <= arg_wrapCount && arg_value.y / arg_range <= arg_wrapCount && arg_value.z / arg_range <= arg_wrapCount;
    isInRange &= 0 < arg_value.x && 0 < arg_value.y && 0 < arg_value.z;
    return isInRange;
}

void GodRayPass(float4 arg_worldColor, inout float4 arg_albedoColor, uint2 arg_launchIndex, CameraEyePosConstData arg_cameraEyePos, LightData arg_lightData, RaytracingAccelerationStructure arg_scene, RWTexture3D<float4> arg_volumeTexture, RaymarchingParam arg_raymarchingParam)
{
    
    //�J��������T���v�����O�n�_�܂ł����C�}�[�`���O�œ������萔�Ŋ���A�T���v�����O�񐔂����߂�B
    const int SAMPLING_COUNT = 16;
    float3 samplingDir = normalize(arg_worldColor.xyz - arg_cameraEyePos.m_eye);
    float samplingLength = length(arg_cameraEyePos.m_eye - arg_worldColor.xyz) / (float) SAMPLING_COUNT;
    float raymarchingBright = 0.0f;
    bool isFinshVolumeFog = false;
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
        
    }
    
    //�{�����[���t�H�O
    float3 fogColor = float3(0,0,0);
    if (arg_raymarchingParam.m_isActive)
    {
        //���C�}�[�`���O�̉񐔂��v�Z�B
        float rayLength = length(arg_cameraEyePos.m_eye - arg_worldColor.xyz);
        float marchingMovedLength = 0; //���C�}�[�`���O�œ���������
        float3 marchingPos = arg_cameraEyePos.m_eye;
        float3 marchingDir = normalize(arg_worldColor.xyz - arg_cameraEyePos.m_eye);
        for (int index = 0; index < 10000; ++index)
        {
        
            //�}�[�`���O��i�߂�ʁB
            float marchingMoveLength = arg_raymarchingParam.m_sampleLength;
        
            //�}�[�`���O��������ړ����Ă�����B
            bool isFinish = false;
            if (rayLength < marchingMovedLength + marchingMoveLength)
            {
            
                //�c��̗ʂ��ړ�������B
                marchingMoveLength = rayLength - marchingMovedLength;
                isFinish = true;

            }
            else
            {
            
                //�������ʂ�ۑ��B
                marchingMovedLength += marchingMoveLength;
            
            }
        
            //�}�[�`���O��i�߂�B
            marchingPos += marchingDir * marchingMoveLength;
        
            //���C�}�[�`���O�̍��W���{�N�Z�����W��Ԃɒ����B
            float3 volumeTexPos = arg_raymarchingParam.m_pos;
            volumeTexPos -= arg_raymarchingParam.m_gridSize * ((256.0f / 2.0f) * arg_raymarchingParam.m_wrapCount);
            int3 boxPos = marchingPos - volumeTexPos; //�}�[�`���O�̃T���v�����O�n�_���{�����[���e�N�X�`���̒��S��̍��W�ɂ��炷�B
            boxPos /= arg_raymarchingParam.m_gridSize;
        
            //�}�[�`���O���W���{�N�Z���̈ʒu��藣��Ă�����T���v�����O���Ȃ��B
            if (!IsInRange(boxPos, 256.0f, arg_raymarchingParam.m_wrapCount))
            {
        
                if (isFinish)
                {
                    break;
                }
                continue;
            }
        
            boxPos.x = boxPos.x % 256;
            boxPos.y = boxPos.y % 256;
            boxPos.z = boxPos.z % 256;
            boxPos = clamp(boxPos, 0, 255);
        
            //�m�C�Y�𔲂����B
            float3 noise = arg_volumeTexture[boxPos].xyz / 50.0f;
        
            float3 weights = float3(0.8f, 0.1f, 0.1f); // �e�m�C�Y�̏d��
            float fogDensity = dot(noise, weights) * arg_raymarchingParam.m_density;
        
            //Y���̍����Ō���������B
            float maxY = 50.0f;
           // fogDensity *= 1.0f - saturate(marchingPos.y / maxY);
        
            //���̕����̐F�𔲂����B
            fogColor += float3(fogDensity, fogDensity, fogDensity) * arg_raymarchingParam.m_color;
            //fogColor = arg_raymarchingParam.m_color * fogDensity + fogColor * saturate(1.0f - fogDensity);
        
            if (isFinish)
            {
                break;
            }
        
        }
    }
    
    const float3 FOGCOLOR_LIT = float3(1.0f, 1.0f, 1.0f);
    const float3 FOGCOLOR_UNLIT = float3(0.0f, 0.0f, 0.0f);
    
    float3 godRayColor = lerp(FOGCOLOR_UNLIT, FOGCOLOR_LIT, raymarchingBright);
    const float FOG_DENSITY = 0.0001f;
    float absorb = exp(-length(arg_cameraEyePos.m_eye - arg_worldColor.xyz) * FOG_DENSITY);
    arg_albedoColor.xyz = lerp(godRayColor, arg_albedoColor.xyz, absorb);
    arg_albedoColor.xyz += float3(clamp(fogColor.x, 0.0f, arg_raymarchingParam.m_color.x), clamp(fogColor.y, 0.0f, arg_raymarchingParam.m_color.y), clamp(fogColor.z, 0.0f, arg_raymarchingParam.m_color.z));
    
}

void SecondaryPass(float3 arg_viewDir, float4 arg_worldColor, float4 arg_materialInfo, float4 arg_normalColor, float4 arg_albedoColor, RaytracingAccelerationStructure arg_scene, CameraEyePosConstData arg_cameraEyePos, inout float4 arg_finalColor)
{
        
    //���C��ID���݂āA���C��ł��ǂ����𔻒f
    if (arg_materialInfo.w == MATERIAL_REFRACT && 0.1f < length(arg_normalColor.xyz))
    {
        
        Payload payloadData;
        payloadData.m_color = float3(1, 1, 1);
        
        //���C������
        float3 rayOrigin = arg_worldColor.xyz + arg_normalColor.xyz * 3.0f;
        CastRay(payloadData, rayOrigin, refract(arg_viewDir, arg_normalColor.xyz, 0.1f), 300000.0f, MISS_DEFAULT, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, arg_scene);
        
        //���ʊi�[
        arg_finalColor = float4(arg_albedoColor.xyz, 1) * arg_materialInfo.y;
        arg_finalColor += float4((payloadData.m_color), 1) * (1.0f - arg_materialInfo.y);
        
    }
    else if (arg_materialInfo.w == MATERIAL_REFLECT && 0.1f < length(arg_normalColor.xyz))
    {
        
        Payload payloadData;
        payloadData.m_color = float3(1, 1, 1);
        
        //���C������
        float3 rayOrigin = arg_worldColor.xyz + arg_normalColor.xyz * 3.0f;
        CastRay(payloadData, rayOrigin, reflect(arg_viewDir, arg_normalColor.xyz), 300000.0f, MISS_DEFAULT, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, arg_scene);
        
        //���ʊi�[
        arg_finalColor = float4(arg_albedoColor.xyz, 1) * arg_materialInfo.y;
        arg_finalColor += float4((payloadData.m_color), 1) * (1.0f - arg_materialInfo.y);
        
    }
    else if (arg_materialInfo.w == MATERIAL_SEA && 0.1f < length(arg_normalColor.xyz))
    {
        
        Payload refractionColor;
        refractionColor.m_color = float3(1, 1, 1);
        Payload reflectionColor;
        reflectionColor.m_color = float3(1, 1, 1);
        
        //���C������
        float3 rayOrigin = arg_worldColor.xyz;
        CastRay(refractionColor, rayOrigin, refract(arg_viewDir, arg_normalColor.xyz, 0.1f), 300000.0f, MISS_CHECKHIT, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, arg_scene);
        CastRay(reflectionColor, rayOrigin, reflect(arg_viewDir, arg_normalColor.xyz), 300000.0f, MISS_CHECKHIT, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, arg_scene);
        
        //���C�������������������Ă��Ȃ����ŐF��ς���B
        if (refractionColor.m_color.x < 0)
        {
            refractionColor.m_color = arg_albedoColor.xyz;
        }
        if (reflectionColor.m_color.x < 0)
        {
            reflectionColor.m_color = float3(0,0,0);
        }
        
        //�C�̐F�̊���
        float perOfSeaColor = (1.0f - arg_materialInfo.y);
        
        //���ʊi�[
        arg_finalColor = float4(arg_albedoColor.xyz, 1) * arg_materialInfo.y;
        arg_finalColor += float4((refractionColor.m_color), 1) * (perOfSeaColor / 2.0f);
        arg_finalColor += float4((reflectionColor.m_color), 1) * (perOfSeaColor / 2.0f);
        
    }
    else
    {
        arg_finalColor = arg_albedoColor;
    }
}