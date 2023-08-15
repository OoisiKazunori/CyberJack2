
struct ParticleData
{
    float3 pos;
    float4 color;
    uint id;
    float timer;
    float maxTimer;
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