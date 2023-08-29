float Lerp(float BASE_POS, float POS, float MUL)
{
    float distance = BASE_POS - POS;
    distance *= MUL;
    
    return POS + distance;
}

float3 Lerp(float3 BASE_POS, float3 POS, float MUL)
{
    float3 distance = BASE_POS - POS;
    distance *= MUL;
    
    return POS + distance;
}

float4 Lerp(float4 BASE_POS, float4 POS, float MUL)
{
    float4 distance = BASE_POS - POS;
    distance *= MUL;
    
    return POS + distance;
}

static const float PI_2 = 3.14f;

float ConvertToRadian(float ANGLE)
{
    return ANGLE * (PI_2 / 180.0f);
}

float RadianToAngle(float RADIAN)
{
    return RADIAN * (180.0f / PI_2);
}

matrix Translate(float3 VECTOR)
{
    matrix matTrans;
    matTrans[0] = float4(1.0f, 0.0f, 0.0f, VECTOR.x);
    matTrans[1] = float4(0.0f, 1.0f, 0.0f, VECTOR.y);
    matTrans[2] = float4(0.0f, 0.0f, 1.0f, VECTOR.z);
    matTrans[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);
    return matTrans;
}
    
matrix Scale(float3 VECTOR)
{
    matrix matScale;
    matScale[0] = float4(VECTOR.x, 0.0f, 0.0f, 0.0f);
    matScale[1] = float4(0.0f, VECTOR.y, 0.0f, 0.0f);
    matScale[2] = float4(0.0f, 0.0f, VECTOR.z, 0.0f);
    matScale[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);
    return matScale;
}
    
matrix RotateX(float ANGLE)
{
    float lsin = sin(ConvertToRadian(ANGLE));
    float lcos = cos(ConvertToRadian(ANGLE));
        
    matrix matRotaX;
    matRotaX[0] = float4(1.0f, 0.0f, 0.0f, 0.0f);
    matRotaX[1] = float4(0.0f, lcos, lsin, 0.0f);
    matRotaX[2] = float4(0.0f, -lsin, lcos, 0.0f);
    matRotaX[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);
    return matRotaX;
}
    
matrix RotateY(float ANGLE)
{
    float lsin = sin(ConvertToRadian(ANGLE));
    float lcos = cos(ConvertToRadian(ANGLE));
    
    matrix matRotaY;
    matRotaY[0] = float4(lcos, 0.0f, -lsin, 0.0f);
    matRotaY[1] = float4(0.0f, 1.0f, 0.0f, 0.0f);
    matRotaY[2] = float4(lsin, 0.0f, lcos, 0.0f);
    matRotaY[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);
    return matRotaY;
}
    
matrix RotateZ(float ANGLE)
{
    float lsin = sin(ConvertToRadian(ANGLE));
    float lcos = cos(ConvertToRadian(ANGLE));
    
    matrix matRotaZ;
    matRotaZ[0] = float4(lcos, lsin, 0.0f, 0.0f);
    matRotaZ[1] = float4(-lsin, lcos, 0.0f, 0.0f);
    matRotaZ[2] = float4(0.0f, 0.0f, 1.0f, 0.0f);
    matRotaZ[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);
    return matRotaZ;
}
    
matrix MatrixIdentity()
{
    matrix matIdentity;
    matIdentity[0] = float4(1.0f, 0.0f, 0.0f, 0.0f);
    matIdentity[1] = float4(0.0f, 1.0f, 0.0f, 0.0f);
    matIdentity[2] = float4(0.0f, 0.0f, 1.0f, 0.0f);
    matIdentity[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);
    return matIdentity;
}

matrix Rotate(float3 ANGLE)
{
    matrix matRot = MatrixIdentity();
    matRot = mul(RotateZ(ANGLE.z), matRot);
    matRot = mul(RotateX(ANGLE.x), matRot);
    matRot = mul(RotateY(ANGLE.y), matRot);
    return matRot;
}

matrix CalucurateWorldMat(float3 POS, float3 SCALE, float3 ROTA, matrix BILLBOARD)
{
    matrix pMatTrans = Translate(POS);
    matrix pMatRot = Rotate(ROTA);
    matrix pMatScale = Scale(SCALE);

    matrix pMatWorld = MatrixIdentity();
    pMatWorld = mul(pMatScale, pMatWorld);
    pMatWorld = mul(pMatRot, pMatWorld);
    pMatWorld = mul(BILLBOARD, pMatWorld);
    pMatWorld = mul(pMatTrans, pMatWorld);

    return pMatWorld;
}

matrix CalucurateWorldMat(float3 POS, float3 SCALE, float3 ROTA)
{
    matrix pMatTrans = Translate(POS);
    matrix pMatRot = Rotate(ROTA);
    matrix pMatScale = Scale(SCALE);

    matrix pMatWorld = MatrixIdentity();
    pMatWorld = mul(pMatScale, pMatWorld);
    pMatWorld = mul(pMatRot, pMatWorld);
    pMatWorld = mul(pMatTrans, pMatWorld);

    return pMatWorld;
}

uint ThreadGroupIndex(uint3 SV_GroupID, uint SV_GroupIndex, uint3 SV_GroupThreadID, int THREAD_INDEX)
{
    uint index = (SV_GroupThreadID.y * THREAD_INDEX) + SV_GroupThreadID.x + SV_GroupThreadID.z;
    index += THREAD_INDEX * SV_GroupID.x;
    return index;
}

//https://www.reedbeta.com/blog/quick-and-easy-gpu-random-numbers-in-d3d11/
uint wang_hash(uint seed)
{
    seed = (seed ^ 61) ^ (seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= 0x27d4eb2d;
    seed = seed ^ (seed >> 15);
    return seed;
}

float3 RandVec3(uint SEED, float MAX, float MIN)
{
    uint rand = wang_hash(SEED * 1847483629);
    float3 result;
    result.x = (rand % 1024) / 1024.0f;
    rand /= 1024;
    result.y = (rand % 1024) / 1024.0f;
    rand /= 1024;
    result.z = (rand % 1024) / 1024.0f;

    result.x = (MAX + abs(MIN)) * result.x - abs(MIN);
    result.y = (MAX + abs(MIN)) * result.y - abs(MIN);
    result.z = (MAX + abs(MIN)) * result.z - abs(MIN);

    if (result.x <= MIN)
    {
        result.x = MIN;
    }
    if (result.y <= MIN)
    {
        result.y = MIN;
    }
    if (result.z <= MIN)
    {
        result.z = MIN;
    }
    return result;
}

struct ParticeArgumentData
{
    float3 pos;
    float3 basePos;
    float3 scale;
    float3 rotation;
    float3 rotationVel;
    float4 color;
    float3 posLerp;
    float3 rotationLerp;
    float4 colorLerp;
    float3 scaleLerp;
    int timer;
    int isHitFlag;
};

static const int PARTICLE_MAX_NUM = 1024;
static const float SCALE = 6.5f;

RWStructuredBuffer<ParticeArgumentData> ParticleDataBuffer : register(u0);
RWStructuredBuffer<uint> RandomTableBuffer : register(u1);
[numthreads(1024, 1, 1)]
void InitCSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex, uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = ThreadGroupIndex(groupId, groupIndex, groupThreadID, 1024);

    //初期位置生�??
    float3 pos;
    const float2 HEIGHT_MAX = float2(200.0f, 0.0f);
    const float2 WIDTH_MAX = float2(400.0f, 200.0f);
    pos.y = RandVec3(RandomTableBuffer[index], HEIGHT_MAX.x, HEIGHT_MAX.y).y;
    pos.z = RandVec3(RandomTableBuffer[index], 2000.0f, 0.0f).z;
    //高さの割合をとってX軸の最低値から値をずらす
    pos.x = WIDTH_MAX.y + (WIDTH_MAX.x - WIDTH_MAX.y) * (pos.y / HEIGHT_MAX.x);
    pos.x += RandVec3(RandomTableBuffer[index], 50.0f, -50.0f).x;
    //左右どちらにつくか
    if (1 <= RandVec3(RandomTableBuffer[index], 2, 0).x)
    {
        pos.x *= -1.0f;
    }
    ParticleDataBuffer[index].pos = pos;
    ParticleDataBuffer[index].posLerp = pos;
    ParticleDataBuffer[index].basePos = pos;
    ParticleDataBuffer[index].scale = float3(SCALE, SCALE, SCALE);
    ParticleDataBuffer[index].rotation = RandVec3(RandomTableBuffer[index], 360.0f, 0.0f);
    ParticleDataBuffer[index].rotationVel = RandVec3(RandomTableBuffer[index], 5.0f, 0.0f);
    ParticleDataBuffer[index].color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    ParticleDataBuffer[index].timer = 0;
    ParticleDataBuffer[index].isHitFlag = 0;
}

struct OutputData
{
    matrix mat;
    float4 color;
};

cbuffer CameraBuffer : register(b0)
{
    matrix billboard;
    matrix viewProj;
    float posZ1;
    float posZ2;
    float posZ3;
    float posZ4;
    float posZ5;
    float posZ6;
    float posZ7;
    float posZ8;
    float posZ9;
    float posZ10;
    float posZ11;
    float posZ12;
    float posZ13;
    float posZ14;
    float posZ15;
    float posZ16;
}

struct VertexBufferData
{
    float3 svpos;
    float3 normal;
    float2 uv;
    float3 tangent;
    float3 binormal;
};


bool CheckCircleAndCircle(float3 arg_A,float3 arg_B,float arg_AR,float arg_BR)
{
    return distance(arg_A,arg_B) <= arg_AR + arg_BR;
};


RWStructuredBuffer<OutputData> WorldDataBuffer : register(u1);
RWStructuredBuffer<matrix> WorldMatBuffer : register(u2);
[numthreads(1024, 1, 1)]
void UpdateCSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex, uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = ThreadGroupIndex(groupId, groupIndex, groupThreadID, 1024);

    float len = 35.0f;
    if(ParticleDataBuffer[index].pos.z - len <= posZ1 && posZ1 <= ParticleDataBuffer[index].pos.z + len)
    {
        ParticleDataBuffer[index].isHitFlag = 1;
    }
    if(ParticleDataBuffer[index].pos.z - len <= posZ2 && posZ2 <= ParticleDataBuffer[index].pos.z + len)
    {
        ParticleDataBuffer[index].isHitFlag = 1;
    }
    if(ParticleDataBuffer[index].pos.z - len <= posZ3 && posZ3 <= ParticleDataBuffer[index].pos.z + len)
    {
        ParticleDataBuffer[index].isHitFlag = 1;
    }
    if(ParticleDataBuffer[index].pos.z - len <= posZ4 && posZ4 <= ParticleDataBuffer[index].pos.z + len)
    {
        ParticleDataBuffer[index].isHitFlag = 1;
    }
    if(ParticleDataBuffer[index].pos.z - len <= posZ5 && posZ5 <= ParticleDataBuffer[index].pos.z + len)
    {
        ParticleDataBuffer[index].isHitFlag = 1;
    }
    if(ParticleDataBuffer[index].pos.z - len <= posZ6 && posZ6 <= ParticleDataBuffer[index].pos.z + len)
    {
        ParticleDataBuffer[index].isHitFlag = 1;
    }
    if(ParticleDataBuffer[index].pos.z - len <= posZ7 && posZ7 <= ParticleDataBuffer[index].pos.z + len)
    {
        ParticleDataBuffer[index].isHitFlag = 1;
    }
    if(ParticleDataBuffer[index].pos.z - len <= posZ8 && posZ8 <= ParticleDataBuffer[index].pos.z + len)
    {
        ParticleDataBuffer[index].isHitFlag = 1;
    }
    if(ParticleDataBuffer[index].pos.z - len <= posZ8 && posZ8 <= ParticleDataBuffer[index].pos.z + len)
    {
        ParticleDataBuffer[index].isHitFlag = 1;
    }

    if(ParticleDataBuffer[index].pos.z - len <= posZ9 && posZ9 <= ParticleDataBuffer[index].pos.z + len)
    {
        ParticleDataBuffer[index].isHitFlag = 1;
    }
    if(ParticleDataBuffer[index].pos.z - len <= posZ10 && posZ10 <= ParticleDataBuffer[index].pos.z + len)
    {
        ParticleDataBuffer[index].isHitFlag = 1;
    }
    if(ParticleDataBuffer[index].pos.z - len <= posZ11 && posZ11 <= ParticleDataBuffer[index].pos.z + len)
    {
        ParticleDataBuffer[index].isHitFlag = 1;
    }
    if(ParticleDataBuffer[index].pos.z - len <= posZ12 && posZ12 <= ParticleDataBuffer[index].pos.z + len)
    {
        ParticleDataBuffer[index].isHitFlag = 1;
    }
    if(ParticleDataBuffer[index].pos.z - len <= posZ13 && posZ13 <= ParticleDataBuffer[index].pos.z + len)
    {
        ParticleDataBuffer[index].isHitFlag = 1;
    }
    if(ParticleDataBuffer[index].pos.z - len <= posZ14 && posZ14 <= ParticleDataBuffer[index].pos.z + len)
    {
        ParticleDataBuffer[index].isHitFlag = 1;
    }
    if(ParticleDataBuffer[index].pos.z - len <= posZ15 && posZ15 <= ParticleDataBuffer[index].pos.z + len)
    {
        ParticleDataBuffer[index].isHitFlag = 1;
    }
    if(ParticleDataBuffer[index].pos.z - len <= posZ16 && posZ16 <= ParticleDataBuffer[index].pos.z + len)
    {
        ParticleDataBuffer[index].isHitFlag = 1;
    }
    
    float3 rotaVel = ParticleDataBuffer[index].rotationVel;
    float4 color = ParticleDataBuffer[index].color;

    if(!ParticleDataBuffer[index].isHitFlag)
    {
        ParticleDataBuffer[index].scale = float3(SCALE, SCALE, SCALE);
        ParticleDataBuffer[index].timer = 0;
        ParticleDataBuffer[index].pos = ParticleDataBuffer[index].basePos;
    }
    else
    {
        ++ParticleDataBuffer[index].timer;
    }

    if(ParticleDataBuffer[index].isHitFlag && ParticleDataBuffer[index].timer <= 5)
    {
        rotaVel = ParticleDataBuffer[index].rotationVel + float3(25.0f,25.0f,25.0f);
        //color = float4(0.90, 0.09, 0.09,1.0);
        ParticleDataBuffer[index].scale = float3(SCALE + 5.0f, SCALE + 5.0f, SCALE + 5.0f);
        //ParticleDataBuffer[index].pos = ParticleDataBuffer[index].basePos + float3(0.0f,55.0f,0.0f);
    }

    if(5 <= ParticleDataBuffer[index].timer)
    {        
        ParticleDataBuffer[index].scale = float3(SCALE + 5.0f, SCALE + 5.0f, SCALE + 5.0f);
        rotaVel = ParticleDataBuffer[index].rotationVel + float3(25.0f,25.0f,25.0f);
        ParticleDataBuffer[index].pos = ParticleDataBuffer[index].basePos;
        //color = float4(0.90, 0.09, 0.09,1.0);
    }
    if(6 <= ParticleDataBuffer[index].timer)
    {
        ParticleDataBuffer[index].scale = float3(SCALE, SCALE, SCALE);
        color = float4(1.0,1.0,1.0,1.0);
        ParticleDataBuffer[index].isHitFlag = 0;
    }

    float lerpVel = 0.1f;
    ParticleDataBuffer[index].rotation += rotaVel;
    ParticleDataBuffer[index].posLerp = lerp(ParticleDataBuffer[index].posLerp,ParticleDataBuffer[index].pos,lerpVel);
    ParticleDataBuffer[index].rotationLerp = lerp(ParticleDataBuffer[index].rotationLerp,ParticleDataBuffer[index].rotation,lerpVel);
    ParticleDataBuffer[index].colorLerp = lerp(ParticleDataBuffer[index].colorLerp,color,lerpVel);
    ParticleDataBuffer[index].scaleLerp = Lerp(ParticleDataBuffer[index].scaleLerp, ParticleDataBuffer[index].scale, lerpVel);

    WorldDataBuffer[index].mat =
    CalucurateWorldMat(
        ParticleDataBuffer[index].posLerp,
        ParticleDataBuffer[index].scaleLerp,
        ParticleDataBuffer[index].rotationLerp
    );

    WorldMatBuffer[index] = WorldDataBuffer[index].mat;
    WorldDataBuffer[index].mat = mul(viewProj,WorldDataBuffer[index].mat);
    WorldDataBuffer[index].color = ParticleDataBuffer[index].colorLerp;
}