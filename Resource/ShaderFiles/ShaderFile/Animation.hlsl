
struct VertexBufferData
{
    float3 pos;
    float3 normal;
    float2 uv;
    float3 tangent;
    float3 binormal;
    int4 boneNo;
    float4 weight;
};

RWStructuredBuffer<VertexBufferData> VertexBuffer : register(u0);
RWStructuredBuffer<VertexBufferData> OutputVertex : register(u1);
RWStructuredBuffer<uint> IndexBuffer : register(u2);

cbuffer BoneBuffer : register(b0)
{
    matrix bones[256];
}
cbuffer VertexNumBuffer : register(b1)
{
    uint vertNum;
}

[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = groupThreadID.x;
    index += 1024 * groupId.x;

    if(vertNum <= index)
    {
        return;
    }

    uint indecies = index;
    VertexBufferData input = VertexBuffer[indecies];

    static const int NO_BONE = -1;
    float4 resultPos = float4(input.pos,1.0f);
    if (input.boneNo[2] != NO_BONE)
    {
        int num;
        
        if (input.boneNo[3] != NO_BONE)
        {
            num = 4;
        }
        else
        {
            num = 3;
        }
        
        matrix mat = bones[input.boneNo[0]] * input.weight[0];
        for (int i = 1; i < num; ++i)
        {
            mat += bones[input.boneNo[i]] * input.weight[i];
        }
        resultPos = mul(mat, float4(input.pos,1.0f));
    }
    else if (input.boneNo[1] != NO_BONE)
    {
        matrix mat = bones[input.boneNo[0]] * input.weight[0];
        mat += bones[input.boneNo[1]] * (1 - input.weight[0]);
        
        resultPos = mul(mat, float4(input.pos,1.0f));
    }
    else if (input.boneNo[0] != NO_BONE)
    {
        resultPos = mul(bones[input.boneNo[0]], float4(input.pos,1.0f));
    }

    OutputVertex[indecies] = input;
    OutputVertex[indecies].pos = resultPos;
}
