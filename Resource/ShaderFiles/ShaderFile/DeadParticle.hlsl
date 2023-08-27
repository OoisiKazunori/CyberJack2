
#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/GPUParticle.hlsli"

float3 Random3D(float3 arg_st)
{
	float3 seed =
		float3(
            dot(arg_st,float3(127.1f, 311.7f, 523.3f)),
			dot(arg_st,float3(269.5f, 183.3f, 497.5f)),
			dot(arg_st,float3(419.2f, 371.9f, 251.6f))
        );
	return float3(-1.0f + 2.0f * frac(sin(seed.x) * 43758.5453123f), -1.0f + 2.0f * frac(sin(seed.y) * 43758.5453123f), -1.0f + 2.0f * frac(sin(seed.z) * 43758.5453123f));
}

float Noise(float3 arg_st)
{
	float3 intValue = { floor(arg_st.x) ,floor(arg_st.y) ,floor(arg_st.z) };
	float3 floatValue = { frac(arg_st.x) ,frac(arg_st.y) ,frac(arg_st.z) };

	//八つの隣接点の座標を求める。
	float3 u;
	u.x = floatValue.x * floatValue.x * (3.0f - 2.0f * floatValue.x);
	u.y = floatValue.y * floatValue.y * (3.0f - 2.0f * floatValue.y);
	u.z = floatValue.z * floatValue.z * (3.0f - 2.0f * floatValue.z);

	//各隣接点でのノイズを求める。
	float center = dot(Random3D(intValue),(floatValue - float3(0, 0, 0)));
	float right = dot(Random3D(intValue + float3(1, 0, 0)),(floatValue - float3(1, 0, 0)));
	float top = dot(Random3D(intValue + float3(0, 1, 0)),(floatValue - float3(0, 1, 0)));
	float rightTop = dot(Random3D(intValue + float3(1, 1, 0)),(floatValue - float3(1, 1, 0)));
	float frontV = dot(Random3D(intValue + float3(0, 0, 1)),(floatValue - float3(0, 0, 1)));
	float rightFront = dot(Random3D(intValue + float3(1, 0, 1)),(floatValue - float3(1, 0, 1)));
	float topFront = dot(Random3D(intValue + float3(0, 1, 1)),(floatValue - float3(0, 1, 1)));
	float rightTopFront = dot(Random3D(intValue + float3(1, 1, 1)),(floatValue - float3(1, 1, 1)));

	//ノイズ値を補間する。
	float x1 = lerp(center, right, u.x);
    float x2 = lerp(top, rightTop, u.x);
    float y1 = lerp(frontV, rightFront, u.x);
    float y2 = lerp(topFront, rightTopFront, u.x);

    float xy1 = lerp(x1, x2, u.y);
    float xy2 = lerp(y1, y2, u.y);

    return lerp(xy1, xy2, u.z);
}

float PerlinNoise(float3 arg_st, int arg_octaves, float arg_persistence, float arg_lacunarity, float3 arg_pos)
{

	float amplitude = 1.0;

	//プレイヤーのワールド座標に基づくノイズ生成
	float3 worldSpaceCoords = arg_st + arg_pos * 0.1f;

	float noiseValue = 0;

	float frequency = 1.0f;
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

float3 CurlNoise3D(float3 arg_st, float3 arg_pos,float arg_vel)
{
	const float epsilon = 0.001f;
	
	//乱数の範囲を-1024 ~ 1024にする。
    arg_st = (arg_st * 2.0f) - float3(1024, 1024, 1024);

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
	
    return vel * arg_vel;

}

struct OutputData
{
    matrix mat;
    float4 color;
};

struct Particle
{
    float3 pos;
    float3 scale;
    float3 localPos;
    int emittTimer;
    int timer;
    int maxTimer;
};

RWStructuredBuffer<ParticleData> updateParticleData : register(u0);
RWStructuredBuffer<Particle> emitterBuffer : register(u1);
RWStructuredBuffer<uint> randomTable : register(u2);

cbuffer cbuff :register(b0)
{
    float3 pos;
    float particleNum;
}

static const float SCALE = 0.25f;

[numthreads(1024, 1, 1)]
void InitCSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex, uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = ThreadGroupIndex(groupId, groupIndex, groupThreadID, 1024);
    
    if(particleNum < index)
    {
        return;
    }

    uint particleIndex = Rand1(randomTable[index],particleNum,0);
    emitterBuffer[index].pos = float3(0,0,0);
    emitterBuffer[index].scale = float3(SCALE,SCALE,SCALE);
    emitterBuffer[index].localPos = updateParticleData[particleIndex].pos;
    emitterBuffer[index].maxTimer = 60;
    emitterBuffer[index].timer = emitterBuffer[index].maxTimer;
    emitterBuffer[index].emittTimer = RandVec3(randomTable[index],60,0).x;
}

cbuffer Camera :register(b0)
{
    matrix viewProjMat;
    matrix billboard;
    matrix scaleRotaMat;
    float4 emittPosAndTimer;
    float4 color;
    float vel;
}

RWStructuredBuffer<OutputData> outputBuffer : register(u0);
RWStructuredBuffer<Particle> particleBuffer : register(u1);
RWStructuredBuffer<uint> curlRandomTable : register(u2);

[numthreads(1024, 1, 1)]
void UpdateCSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex, uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = ThreadGroupIndex(groupId, groupIndex, groupThreadID, 1024);

    float rate = (float)particleBuffer[index].timer / (float)particleBuffer[index].maxTimer;
    //パーティクル開始時間
    if(emittPosAndTimer.w < particleBuffer[index].emittTimer)
    {
        particleBuffer[index].pos = GetPos(particleBuffer[index].localPos,emittPosAndTimer.xyz,float3(1.0f,1.0f,1.0f)).xyz;
        return;
    }

    
    particleBuffer[index].pos += CurlNoise3D(float3(curlRandomTable[index],curlRandomTable[index],curlRandomTable[index]),particleBuffer[index].pos,vel);
    if(0 < particleBuffer[index].timer)
    {
        --particleBuffer[index].timer;
    }
    else
    {
        particleBuffer[index].timer = 0;
    }
    outputBuffer[index].color = color;
    float scaleRate = SCALE * rate;
    particleBuffer[index].scale = float3(scaleRate,scaleRate,scaleRate);

    matrix worldMat = CalucurateWorldMat(
        particleBuffer[index].pos,particleBuffer[index].scale,float3(0,0,0),billboard
    );
    outputBuffer[index].mat = mul(viewProjMat,worldMat);
}