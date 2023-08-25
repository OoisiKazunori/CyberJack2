
struct ParticleData
{
    float3 pos;
    float4 color;
    uint id;
    float timer;
    float maxTimer;
    float2 rate;
    uint3 vertexIndex;
    uint2 lengthIndex;
};

struct ParticleHitData
{
    float3 pos;
    float4 color;
    uint id;
    uint hitFlag;
    uint hitTimer;
};


struct GPUParticleInput
{
    matrix worldMat;
    float4 color;
};