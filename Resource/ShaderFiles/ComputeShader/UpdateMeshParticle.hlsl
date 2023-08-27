#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/GPUParticle.hlsli"
float Lerp(float BASE_POS,float POS,float MUL)
{
    float distance = BASE_POS - POS;
	distance *= MUL;
    
    return POS + distance;
}

float3 Lerp(float3 BASE_POS,float3 POS,float MUL)
{
    float3 distance = BASE_POS - POS;
	distance *= MUL;
    
    return POS + distance;
}

float4 Lerp(float4 BASE_POS,float4 POS,float MUL)
{
    float4 distance = BASE_POS - POS;
	distance *= MUL;
    
    return POS + distance;
}

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

float3 CurlNoise3D(float3 arg_st, float3 arg_pos)
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
	
    return vel * 10.0f;

}

struct VertexBufferData
{
    float3 svpos;
    float3 normal;
    float2 uv;
    float3 tangent;
    float3 binormal;
	uint4 boneNo;
	float4 weight;
};

float3 GetNormal(RWStructuredBuffer<VertexBufferData> vertex,uint index,uint2 offset)
{
    return cross(normalize(vertex[index + offset.x].svpos - vertex[index].svpos),normalize(vertex[index + offset.y].svpos - vertex[index].svpos));
}

void GenerateVertexData(inout RWStructuredBuffer<VertexBufferData> bufferData,int index,matrix worldMat)
{
    uint vertexIndex = index * 4;

    bufferData[index].svpos = float4(-0.5f, 0.5f, 0.0f, 1.0f);
    bufferData[index + 1].svpos = float4(-0.5f, -0.5f, 0.0f, 1.0f);
    bufferData[index + 2].svpos = float4(0.5f, 0.5f, 0.0f, 1.0f);
    bufferData[index + 3].svpos = float4(0.5f, -0.5f, 0.0f, 1.0f);

    bufferData[index].svpos =     mul(worldMat, float4(bufferData[index].svpos    , 1.0f));
    bufferData[index + 1].svpos = mul(worldMat, float4(bufferData[index + 1].svpos, 1.0f));
    bufferData[index + 2].svpos = mul(worldMat, float4(bufferData[index + 2].svpos, 1.0f));
    bufferData[index + 3].svpos = mul(worldMat, float4(bufferData[index + 3].svpos, 1.0f));

    bufferData[index].normal =     GetNormal(bufferData,index,uint2(1,2));
    bufferData[index + 1].normal = GetNormal(bufferData,index,uint2(1,2));
    bufferData[index + 2].normal = GetNormal(bufferData,index,uint2(1,2));
    bufferData[index + 3].normal = GetNormal(bufferData,index,uint2(1,2));
}

void GenerateIndexData(inout RWStructuredBuffer<uint> bufferData,int index)
{
    uint indeciesIndex = index * 6;
    if(index == 0)
    {
        bufferData[indeciesIndex] = indeciesIndex;
	    bufferData[indeciesIndex + 1] = indeciesIndex + 1;
	    bufferData[indeciesIndex + 2] = indeciesIndex + 2;
	    bufferData[indeciesIndex + 3] = indeciesIndex + 2;
	    bufferData[indeciesIndex + 4] = indeciesIndex + 1;
	    bufferData[indeciesIndex + 5] = indeciesIndex + 3;
    }
    else
    {
        uint offsetIndex = index * 4;
        bufferData[indeciesIndex] = offsetIndex;
	    bufferData[indeciesIndex + 1] = offsetIndex + 1;
	    bufferData[indeciesIndex + 2] = offsetIndex + 2;
	    bufferData[indeciesIndex + 3] = offsetIndex + 2;
	    bufferData[indeciesIndex + 4] = offsetIndex + 1;
	    bufferData[indeciesIndex + 5] = offsetIndex + 3;
    }
}



struct MotherMatData
{
	matrix motherMat;
};

//����
RWStructuredBuffer<ParticleData> updateParticleData : register(u0);
RWStructuredBuffer<MotherMatData> motherMatData : register(u1);
RWStructuredBuffer<matrix> scaleRotaMatData : register(u2);
RWStructuredBuffer<float> alphaData : register(u3);
//�o��
AppendStructuredBuffer<GPUParticleInput> inputGPUParticleData : register(u4);

RWStructuredBuffer<uint> randomTable : register(u5);
RWStructuredBuffer<uint> curlNoizeBuffer : register(u6);

RWStructuredBuffer<VertexBufferData> vertexBuffer : register(u7);

cbuffer Camera :register(b0)
{
    matrix viewProjMat;
    matrix billboard;
	uint indexNum;
}

[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = groupThreadID.x;
    index += 1024 * groupId.x;

    ParticleData particleData = updateParticleData[index];

	if(curlNoizeBuffer[particleData.id])
	{
    	updateParticleData[index].color.a = (float)updateParticleData[index].timer / (float)updateParticleData[index].maxTimer;
        float timerRate = 1.0f - updateParticleData[index].color.a;
    	updateParticleData[index].pos += CurlNoise3D(float3(randomTable[index],randomTable[index],randomTable[index]),updateParticleData[index].pos) * clamp(timerRate, 0.3f, 1.0f);
    	if(0 < updateParticleData[index].timer)
    	{
    	    --updateParticleData[index].timer;
    	}
    	else
    	{
    	    updateParticleData[index].color.a = 0;
    	}
	}
	else
	{
		uint firstVertIndex = particleData.vertexIndex.x;
    	uint secondVertIndex = particleData.vertexIndex.y;
		uint thirdVertIndex = particleData.vertexIndex.z;
		float3 firstVertWorldPos = GetPos(vertexBuffer[firstVertIndex].svpos.xyz,float3(0,0,0),float3(0,0,0));
    	float3 secondVertWorldPos = GetPos(vertexBuffer[secondVertIndex].svpos.xyz,float3(0,0,0),float3(0,0,0));
    	float3 thirdVertWorldPos = GetPos(vertexBuffer[thirdVertIndex].svpos.xyz,float3(0,0,0),float3(0,0,0));
    	float3 triangleCentralPos = (firstVertWorldPos.xyz + secondVertWorldPos.xyz + thirdVertWorldPos.xyz) / 3.0f;
		//辺上の計算
		float3 distance = vertexBuffer[particleData.lengthIndex.y].svpos.xyz - vertexBuffer[particleData.lengthIndex.x].svpos.xyz;
		float3 edge = vertexBuffer[particleData.lengthIndex.x].svpos.xyz + distance * particleData.rate.x;
		//面上の計算
		distance = triangleCentralPos - edge;
		float3 area = edge + distance * particleData.rate.y;
		updateParticleData[index].pos = area;
	}
    matrix worldMat = mul(scaleRotaMatData[particleData.id],billboard);
    worldMat[0][3] = updateParticleData[index].pos.x;
    worldMat[1][3] = updateParticleData[index].pos.y;
    worldMat[2][3] = updateParticleData[index].pos.z;
    worldMat = mul(motherMatData[particleData.id].motherMat,worldMat);

    GPUParticleInput inputData;
    inputData.worldMat = mul(viewProjMat,worldMat);
    inputData.color = particleData.color * float4(1.0f,1.0f,1.0f,alphaData[particleData.id]);
    inputGPUParticleData.Append(inputData);
}
