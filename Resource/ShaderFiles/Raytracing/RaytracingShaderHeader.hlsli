
//�~����
static const float PI = 3.141592653589f;

//�J�����p�̒萔�o�b�t�@
struct CameraConstBufferData
{
    matrix mtxView; //�r���[�s��
    matrix mtxProj; //�v���W�F�N�V�����s��
    matrix mtxViewInv; //�t�r���[�s��
    matrix mtxProjInv; //�t�v���W�F�N�V�����s��
};

//�萔�o�b�t�@
struct ConstBufferData
{
    CameraConstBufferData camera;
};

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
    float3 color_; //�F���
};

struct MyAttribute
{
    float2 barys;
};

//barys���v�Z
inline float3 CalcBarycentrics(float2 Barys)
{
    return float3(1.0 - Barys.x - Barys.y, Barys.x, Barys.y);
}

//���������ʒu�̏����擾����֐�
Vertex GetHitVertex(MyAttribute attrib, StructuredBuffer<Vertex> vertexBuffer, StructuredBuffer<uint> indexBuffer, inout Vertex meshInfo[3])
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
        
        //���b�V���̏���ۑ��B
        meshInfo[index].pos = mul(float4(vertexBuffer[vtxIndex].pos, 1), ObjectToWorld4x3());
        meshInfo[index].normal = normalize(mul(vertexBuffer[vtxIndex].normal, (float3x3) ObjectToWorld4x3()));
        meshInfo[index].uv = vertexBuffer[vtxIndex].uv;
    }

    return v;
}

//�w��̒��_�̏Փ˂������b�V����ł̏d�S���W�����߂�B
float3 CalcVertexBarys(float3 HitVertex, float3 VertexA, float3 VertexB, float3 VertexC)
{
    
    float3 e0 = VertexB - VertexA;
    float3 e1 = VertexC - VertexA;
    float3 e2 = HitVertex - VertexA;
    float d00 = dot(e0, e0);
    float d01 = dot(e0, e1);
    float d11 = dot(e1, e1);
    float d20 = dot(e2, e0);
    float d21 = dot(e2, e1);
    float denom = 1.0 / (d00 * d11 - d01 * d01);
    float v = (d11 * d20 - d01 * d21) * denom;
    float w = (d00 * d21 - d01 * d20) * denom;
    float u = 1.0 - v - w;
    return float3(u, v, w);
    
}