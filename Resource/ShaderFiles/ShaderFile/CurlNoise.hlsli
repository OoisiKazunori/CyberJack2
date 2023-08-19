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
	float x1 = Lerp(center, right, u.x);
	float x2 = Lerp(top, rightTop, u.x);
	float y1 = Lerp(frontV, rightFront, u.x);
	float y2 = Lerp(topFront, rightTopFront, u.x);

	float xy1 = Lerp(x1, x2, u.y);
	float xy2 = Lerp(y1, y2, u.y);

	return Lerp(xy1, xy2, u.z);
}

float PerlinNoise(float3 arg_st, int arg_octaves, float arg_persistence, float arg_lacunarity, float3 arg_pos)
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

float3 CurlNoise3D(float3 arg_st, float3 arg_pos)
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