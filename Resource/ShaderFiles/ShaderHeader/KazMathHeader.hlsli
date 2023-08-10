
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

//�X�v���C���Ȑ�
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

//�X���b�h�̈�������0~�X���b�h�̍ő吔���v�Z���܂�
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


float3 CurlNoise3D(const float3& arg_st, const float3& arg_pos)
{

	const float epsilon = 0.01f;

	int octaves = 4; //オクターブ数
	float persistence = 0.5; //持続度
	float lacunarity = 2.0f; //ラクナリティ

	//ノイズの中心
	float noiseCenter = PerlinNoise(arg_st, octaves, persistence, lacunarity, arg_pos);

	//各軸にちょっとだけずらした値を求める。x + h
	float noiseX = PerlinNoise(arg_st, octaves, persistence, lacunarity, arg_pos + float3(epsilon, 0, 0));
	float noiseY = PerlinNoise(arg_st, octaves, persistence, lacunarity, arg_pos + float3(0, epsilon, 0));
	float noiseZ = PerlinNoise(arg_st, octaves, persistence, lacunarity, arg_pos + float3(0, 0, epsilon));

	//微分を求める。 f(x + h) - f(x) / h
	float dNoiseX = (noiseX - noiseCenter) / epsilon;
	float dNoiseY = (noiseY - noiseCenter) / epsilon;
	float dNoiseZ = (noiseZ - noiseCenter) / epsilon;

	//ベクトルを回転させる。
	float3 vel;
	vel.x = dNoiseY - dNoiseZ;
	vel.y = dNoiseZ - dNoiseX;
	vel.z = dNoiseX - dNoiseY;

	return vel;

}

float PlayerMoveParticleIdle::Frac(float arg_x)
{
	float intpart;
	float fracpart = std::modf(arg_x, &intpart);
	return fracpart;
}

float3 PlayerMoveParticleIdle::Random3D(float3 arg_st)
{
	float3 seed =
		float3(arg_st.Dot(float3(127.1f, 311.7f, 523.3f)),
			arg_st.Dot(float3(269.5f, 183.3f, 497.5f)),
			arg_st.Dot(float3(419.2f, 371.9f, 251.6f)));
	return float3(-1.0f + 2.0f * Frac(sinf(seed.x) * 43758.5453123f), -1.0f + 2.0f * Frac(sinf(seed.y) * 43758.5453123f), -1.0f + 2.0f * Frac(sinf(seed.z) * 43758.5453123f));
}

float PlayerMoveParticleIdle::Noise(float3 arg_st)
{
	float3 intValue = { std::floor(arg_st.x) ,std::floor(arg_st.y) ,std::floor(arg_st.z) };
	float3 floatValue = { Frac(arg_st.x) ,Frac(arg_st.y) ,Frac(arg_st.z) };

	//八つの隣接点の座標を求める。
	float3 u;
	u.x = floatValue.x * floatValue.x * (3.0f - 2.0f * floatValue.x);
	u.y = floatValue.y * floatValue.y * (3.0f - 2.0f * floatValue.y);
	u.z = floatValue.z * floatValue.z * (3.0f - 2.0f * floatValue.z);

	//各隣接点でのノイズを求める。
	float center = Random3D(intValue).Dot(floatValue - float3(0, 0, 0));
	float right = Random3D(intValue + float3(1, 0, 0)).Dot(floatValue - float3(1, 0, 0));
	float top = Random3D(intValue + float3(0, 1, 0)).Dot(floatValue - float3(0, 1, 0));
	float rightTop = Random3D(intValue + float3(1, 1, 0)).Dot(floatValue - float3(1, 1, 0));
	float front = Random3D(intValue + float3(0, 0, 1)).Dot(floatValue - float3(0, 0, 1));
	float rightFront = Random3D(intValue + float3(1, 0, 1)).Dot(floatValue - float3(1, 0, 1));
	float topFront = Random3D(intValue + float3(0, 1, 1)).Dot(floatValue - float3(0, 1, 1));
	float rightTopFront = Random3D(intValue + float3(1, 1, 1)).Dot(floatValue - float3(1, 1, 1));

	//ノイズ値を補間する。
	float x1 = Lerp(center, right, u.x);
	float x2 = Lerp(top, rightTop, u.x);
	float y1 = Lerp(front, rightFront, u.x);
	float y2 = Lerp(topFront, rightTopFront, u.x);

	float xy1 = Lerp(x1, x2, u.y);
	float xy2 = Lerp(y1, y2, u.y);

	return Lerp(xy1, xy2, u.z);
}

float PlayerMoveParticleIdle::PerlinNoise(float3 arg_st, int arg_octaves, float arg_persistence, float arg_lacunarity, float3 arg_pos)
{

	float amplitude = 1.0;

	//プレイヤーのワールド座標に基づくノイズ生成
	float3 worldSpaceCoords = arg_st + arg_pos / 100.0f;

	float noiseValue = 0;

	float frequency = 2.0f;
	float localAmplitude = amplitude;
	float sum = 0.0;
	float maxValue = 0.0;

	for (int i = 0; i < arg_octaves; ++i)
	{
		sum += localAmplitude * Noise(worldSpaceCoords * frequency);
		maxValue += localAmplitude;

		localAmplitude *= arg_persistence;
		frequency *= arg_lacunarity;
	}

	noiseValue = (sum / maxValue + 1.0f) * 0.5f; //ノイズ値を0.0から1.0の範囲に再マッピング


	return noiseValue;

}