
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
    matRot = mul(RotateZ(ANGLE.z),matRot);
    matRot = mul(RotateX(ANGLE.x),matRot);
    matRot = mul(RotateY(ANGLE.y),matRot);
    return matRot;
}

float Rand(int SEED,int SEED2,int MAX,int MIN)
{
    float rand = frac(sin(dot(float2(SEED,SEED), float2(12.9898, 78.233)) + (SEED2 + SEED)) * 43758.5453);
    return (MAX + abs(MIN)) * rand - abs(MIN);
}

float AngleToRadian(float ANGLE)
{
	float radian = ANGLE * (3.14f / 180.0f);
	return radian;
}

static float3 K = float3(0x456789abu, 0x6789ab45u, 0x89ab4567u); //large odd digits, 0 and 1 are fully mixed)

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

float3 RandVec3(uint SEED,float MAX,float MIN)
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

    if(result.x <= MIN)
    {
        result.x = MIN;        
    }
    if(result.y <= MIN)
    {
        result.y = MIN;        
    }
    if(result.z <= MIN)
    {
        result.z = MIN;        
    }
    return result;
}

float Rand1(uint SEED,int MAX,int MIN)
{
    uint rand = wang_hash(SEED * 1847483629);
    float result;
    result = (rand % 1024) / 1024.0f;
    result = (MAX + abs(MIN)) * result - abs(MIN);
    return result;
}

matrix SetPosInMat(matrix MAT,float3 POS)
{
    MAT[0][3] = POS.x;
    MAT[1][3] = POS.y;
    MAT[2][3] = POS.z;
    return MAT;
}

matrix CalucurateWorldMat(float3 POS,float3 SCALE,float3 ROTA)
{
    matrix pMatTrans = Translate(POS);
    matrix pMatRot = Rotate(ROTA);
    matrix pMatScale = Scale(SCALE);

    matrix pMatWorld = MatrixIdentity();
    pMatWorld = mul(pMatScale, pMatWorld);
    pMatWorld = mul(pMatRot,   pMatWorld);
    pMatWorld = mul(pMatTrans, pMatWorld);

    return pMatWorld;
}

matrix CalucurateWorldMat(float3 POS,float3 SCALE,float3 ROTA,matrix BILLBOARD)
{
    matrix pMatTrans = Translate(POS);
    matrix pMatRot = Rotate(ROTA);
    matrix pMatScale = Scale(SCALE);

    matrix pMatWorld = MatrixIdentity();
    pMatWorld = mul(pMatScale, pMatWorld);
    pMatWorld = mul(pMatRot,   pMatWorld);
    pMatWorld = mul(BILLBOARD, pMatWorld);
    pMatWorld = mul(pMatTrans, pMatWorld);

    return pMatWorld;
}

matrix CalucurateMat(matrix WORLD,matrix VIEW,matrix PROJ)
{
    matrix mat;
    mat = MatrixIdentity();
    mat = mul(WORLD,mat);
    mat = mul(VIEW, mat);
    mat = mul(PROJ, mat);
    return mat;
}

float CaluDistacne(float3 POS_A, float3 POS_B)
{
    float3 distance = float3(abs(POS_B.x - POS_A.x),abs(POS_B.y - POS_A.y),abs(POS_B.z - POS_A.z));
    float3 lpow;
	lpow.x = pow(distance.x, 2.0f);
	lpow.y = pow(distance.y, 2.0f);
	lpow.z = pow(distance.z, 2.0f);

	return sqrt(lpow.x + lpow.y + lpow.z);
}

float4 GetPos(float3 VERT_POS,float3 WORLD_POS)
{
    float3 defaltScale = float3(1,1,1);
    float3 defaltRota = float3(0,0,0);

    matrix pMatWorld = CalucurateWorldMat(WORLD_POS,defaltScale,defaltRota);
    matrix vertMatWorld = CalucurateWorldMat(VERT_POS,defaltScale,defaltRota);

    matrix worldMat = mul(vertMatWorld,pMatWorld);

    return float4(worldMat[0].w,worldMat[1].w,worldMat[2].w,0.0f);
};

float4 GetPos(float3 VERT_POS,float3 WORLD_POS,matrix SCALE_ROTA_MAT)
{
    float3 defaltRota = float3(0,0,0);

    matrix pMatWorld = SetPosInMat(SCALE_ROTA_MAT,WORLD_POS);
    matrix vertMatWorld = SetPosInMat(SCALE_ROTA_MAT,VERT_POS);

    matrix worldMat = mul(vertMatWorld,pMatWorld);

    return float4(worldMat[0].w,worldMat[1].w,worldMat[2].w,0.0f);
};

//スプライン曲線
float4 SplinePosition(RWStructuredBuffer<float3> LIMIT_INDEX_ARRAY,int START_INDEX,float RATE,int INDEX_MAX)
{
    if (START_INDEX < 1)
	{
		return float4(LIMIT_INDEX_ARRAY[1].xyz,0);
	}
    int n = INDEX_MAX - 2;
    if(n < START_INDEX)
    {
        return float4(LIMIT_INDEX_ARRAY[1].xyz,0);
    }
	float3 p0 = LIMIT_INDEX_ARRAY[START_INDEX - 1].xyz;
	float3 p1 = LIMIT_INDEX_ARRAY[START_INDEX].xyz;
	float3 p2 = LIMIT_INDEX_ARRAY[START_INDEX + 1].xyz;
	float3 p3 = LIMIT_INDEX_ARRAY[START_INDEX + 2].xyz;

    float3 resultPos;
    resultPos = 0.5 * ((2 * p1  + (-p0 + p2) * RATE) + (2 * p0 - 5 * p1 + 4 * p2 - p3) * (RATE * RATE) + (-p0 + 3 * p1 - 3 * p2 + p3) * (RATE * RATE * RATE));
    return float4(resultPos.xyz,0);
};

float3 Larp(float3 BASE_POS,float3 POS,float MUL)
{
    float3 distance = BASE_POS - POS;
	distance *= MUL;
    
    return POS + distance;
}

float4 Larp(float4 BASE_POS,float4 POS,float MUL)
{
    float4 distance = BASE_POS - POS;
	distance *= MUL;
    
    return POS + distance;
}

//スレッドの引数から0~スレッドの最大数を計算します
uint ThreadGroupIndex(uint3 SV_GroupID, uint SV_GroupIndex,uint3 SV_GroupThreadID,int THREAD_INDEX)
{
    uint index = (SV_GroupThreadID.y * THREAD_INDEX) + SV_GroupThreadID.x + SV_GroupThreadID.z;
    index += THREAD_INDEX * SV_GroupID.x;
    return index;
}

uint ThreadGroupIndex(uint3 SV_GroupID,int X_MAX,int XY_MAX)
{
    return SV_GroupID.x + SV_GroupID.y * X_MAX + SV_GroupID.z * XY_MAX;
}

float3 MinFloat3(float3 NUM_1,float3 NUM_2)
{
    float3 num;
    num.x = min(NUM_1.x,NUM_2.x);
    num.y = min(NUM_1.y,NUM_2.y);
    num.z = min(NUM_1.z,NUM_2.z);
    return num;
}

float3 MaxFloat3(float3 NUM_1,float3 NUM_2)
{
    float3 num;
    num.x = max(NUM_1.x,NUM_2.x);
    num.y = max(NUM_1.y,NUM_2.y);
    num.z = max(NUM_1.z,NUM_2.z);
    return num;
}

float3 UpdatePos(matrix MOTHER_MAT,float3 POS)
{
    matrix worldMat = MatrixIdentity();
    worldMat[0][3] = POS.x;
    worldMat[1][3] = POS.y;
    worldMat[2][3] = POS.z;
    worldMat = mul(MOTHER_MAT,worldMat);

    float3 resultPos;    
    resultPos.x = worldMat[0][3];
    resultPos.y = worldMat[1][3];
    resultPos.z = worldMat[2][3];
    return resultPos;
}