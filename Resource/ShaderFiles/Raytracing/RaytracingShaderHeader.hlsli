
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


void GodRayPass(float4 arg_worldColor, inout float4 arg_albedoColor, CameraEyePosConstData arg_cameraEyePos, LightData arg_lightData, RaytracingAccelerationStructure arg_scene)
{
    
    //�J��������T���v�����O�n�_�܂ł����C�}�[�`���O�œ������萔�Ŋ���A�T���v�����O�񐔂����߂�B
    const int SAMPLING_COUNT = 16;
    float3 samplingDir = normalize(arg_worldColor.xyz - arg_cameraEyePos.m_eye);
    float samplingLength = length(arg_cameraEyePos.m_eye - arg_worldColor.xyz) / (float) SAMPLING_COUNT;
    float raymarchingBright = 0.0f;
    for (int counter = 0; counter < SAMPLING_COUNT; ++counter)
    {
                
        //�y�C���[�h(�ċA�I�ɏ��������郌�C�g���̒��Œl�̎󂯓n���Ɏg�p����\����)��錾�B
        Payload payloadData;
        payloadData.m_color = float3(0.0f, 0.0f, 0.0f); //�F��^�����ɂ��Ă����B���C���΂��Ăǂ��ɂ�������Ȃ��������ɌĂ΂��MissShader���Ă΂ꂽ�炻����1���������ށB�����ɓ��������Ƃ��ɌĂ΂��ClosestHitShader���Ă΂ꂽ�炻���͉e�Ȃ̂�0���������ށB
        
        //�f�B���N�V�������C�g�̏���
        if (arg_lightData.m_dirLight.m_isActive)
        {
        
            //���C������
            CastRay(payloadData, arg_cameraEyePos.m_eye + (samplingDir * samplingLength) * counter, -arg_lightData.m_dirLight.m_dir, 300000.0f, MISS_LIGHTING, RAY_FLAG_SKIP_CLOSEST_HIT_SHADER, arg_scene);
        
            //���ʂ�ۑ��B
            float progress = 1.0f / float(counter + 1);
            raymarchingBright = lerp(raymarchingBright, payloadData.m_color.x, progress);
            
        }
        //�|�C���g���C�g�̏���
        float pointLightDistance = length((arg_cameraEyePos.m_eye + (samplingDir * samplingLength) * counter) - arg_worldColor.xyz);
        if (arg_lightData.m_pointLight.m_isActive && pointLightDistance < arg_lightData.m_pointLight.m_power)
        {
        
            //���C������
            CastRay(payloadData, arg_cameraEyePos.m_eye + (samplingDir * samplingLength) * counter, -arg_lightData.m_dirLight.m_dir, pointLightDistance, MISS_LIGHTING, RAY_FLAG_SKIP_CLOSEST_HIT_SHADER, arg_scene);
        
            //���ʂ�ۑ��B
            float progress = 1.0f / float(counter + 1);
            raymarchingBright = lerp(raymarchingBright, payloadData.m_color.x, progress);
            
        }
        
    }
    const float3 FOGCOLOR_LIT = float3(1.0f, 1.0f, 1.0f);
    const float3 FOGCOLOR_UNLIT = float3(0.0f, 0.0f, 0.0f);
    
    float3 fogColor = lerp(FOGCOLOR_UNLIT, FOGCOLOR_LIT, raymarchingBright);
    const float FOG_DENSITY = 0.0001f;
    float absorb = exp(-length(arg_cameraEyePos.m_eye - arg_worldColor.xyz) * FOG_DENSITY);
    arg_albedoColor.xyz = lerp(fogColor, arg_albedoColor.xyz, absorb);
    
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

//�������擾�B
float3 Random3D(float3 arg_st)
{
    float3 seed = float3(dot(arg_st, float3(127.1f, 311.7f, 523.3f)), dot(arg_st, float3(269.5f, 183.3f, 497.5f)), dot(arg_st, float3(419.2f, 371.9f, 251.6f)));
    return -1.0f + 2.0f * frac(sin(seed) * 43758.5453123f);
}

//3D�O���f�B�G���g�m�C�Y�֐�
float GradientNoise(float3 arg_st)
{
    float3 i = floor(arg_st);
    float3 f = frac(arg_st);

    //���̗אړ_�̍��W�����߂�
    float3 u = f * f * (3.0 - 2.0 * f);

    float a = dot(Random3D(i), f - float3(0, 0, 0));
    float b = dot(Random3D(i + float3(1, 0, 0)), f - float3(1, 0, 0));
    float c = dot(Random3D(i + float3(0, 1, 0)), f - float3(0, 1, 0));
    float d = dot(Random3D(i + float3(1, 1, 0)), f - float3(1, 1, 0));
    float e = dot(Random3D(i + float3(0, 0, 1)), f - float3(0, 0, 1));
    float f1 = dot(Random3D(i + float3(1, 0, 1)), f - float3(1, 0, 1));
    float g = dot(Random3D(i + float3(0, 1, 1)), f - float3(0, 1, 1));
    float h = dot(Random3D(i + float3(1, 1, 1)), f - float3(1, 1, 1));

    //�m�C�Y�l���Ԃ���
    float x1 = lerp(a, b, u.x);
    float x2 = lerp(c, d, u.x);
    float y1 = lerp(e, f1, u.x);
    float y2 = lerp(g, h, u.x);

    float xy1 = lerp(x1, x2, u.y);
    float xy2 = lerp(y1, y2, u.y);

    return lerp(xy1, xy2, u.z);
}

//3D�p�[�����m�C�Y�֐��i���̕\���t���j
float3 PerlinNoiseWithWind(float3 arg_st, int arg_octaves, float arg_persistence, float arg_lacunarity, float arg_windStrength, float arg_windSpeed, float arg_timer, float3 arg_worldPos, float arg_threshold)
{
    float amplitude = 1.0f;

    //���̉e�����v�Z
    float3 windDirection = normalize(float3(1, 0, 0)); //���̕�����ݒ�i���̏ꍇ�� (1, 0, 0) �̕����j
    float3 windEffect = windDirection * arg_windStrength * (arg_timer * arg_windSpeed);

    //�v���C���[�̃��[���h���W�Ɋ�Â��m�C�Y����
    //float3 worldSpaceCoords = arg_st + arg_worldPos / 100.0f;
    float3 worldSpaceCoords = arg_st;

    float3 noiseValue = float3(0, 0, 0);

    for (int j = 0; j < 3; ++j)
    {
        float frequency = pow(2.0f, float(j));
        float localAmplitude = amplitude;
        float sum = 0.0f;
        float maxValue = 0.0f;
        
        for (int i = 0; i < arg_octaves; ++i)
        {
            sum += localAmplitude * GradientNoise((worldSpaceCoords + windEffect) * frequency + (arg_timer + windEffect.x)); //�X���[�Y�Ȏ��ԕϐ��ƕ��̉e�����m�C�Y�֐��ɓK�p
            maxValue += localAmplitude;

            localAmplitude *= arg_persistence;
            frequency *= arg_lacunarity;
        }

        noiseValue[j] = (sum / maxValue + 1.0f) * 0.5f; //�m�C�Y�l��0.0����1.0�͈̔͂ɍă}�b�s���O

        if (noiseValue[j] <= arg_threshold)
        {
            noiseValue[j] = 0.0f;
        }
    }

    return noiseValue;
}





//�t���N�^���m�C�Y
float FBM(float2 arg_st)
{
    float result = 0.0f;
    float amplitude = 1.0f;  //�U��

    for (int counter = 0; counter < 5; counter++)
    {
        result += amplitude * GradientNoise(float3(arg_st, 1.0f));
        amplitude *= 0.5f;   //�U�������炷�B��������ƃm�C�Y���ׂ����Ȃ��Ă����B
        arg_st *= 2.0f;      //���g���������Ă����B    
    }

    return result;
}

//�h���C�����[�s���O
float3 DomainWarping(float2 arg_st, float arg_time)
{

    float3 color = float3(1.0f, 1.0f, 1.0f);

    //�ŏ��̈���
    float2 q = float2(0.0f, 0.0f);
    q.x = FBM(arg_st + float2(0.0f, 0.0f));
    q.y = FBM(arg_st + float2(1.0f, 1.0f));

    //�ŏ��̈���������ɉ��H�B
    float2 r = float2(0.0f, 0.0f);
    r.x = FBM(arg_st + (4.0f * q) + float2(1.7f, 9.2f) + (0.15f * arg_time));
    r.y = FBM(arg_st + (4.0f * q) + float2(8.3f, 2.8f) + (0.12f * arg_time));
    
    //�F�����߂�B
    float3 mixColor1 = float3(0.8f, 0.35f, 0.12f);
    float3 mixColor2 = float3(0.3f, 0.75f, 0.69f);
    color = lerp(color, mixColor1, clamp(length(q), 0.0f, 1.0f));
    color = lerp(color, mixColor2, clamp(length(r), 0.0f, 1.0f));

    //�O�i�K�ڂ̃m�C�Y���擾�B
    float f = FBM(arg_st + 4.0f * r);

    //���ʂ�g�ݍ��킹��B
    float coef = (f * f * f + (0.6f * f * f) + (0.5f * f)) * 10.0f;
    return color * coef;
    
}