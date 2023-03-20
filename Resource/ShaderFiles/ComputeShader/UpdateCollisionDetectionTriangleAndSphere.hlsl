#include"../ShaderHeader/KazMathHeader.hlsli"
#include"../ShaderHeader/ExcuteIndirectHeader.hlsli"

float3 ClosestPoint(float3 POINT_POS, float3 TRIANGLE_A_POS, float3 TRIANGLE_B_POS, float3 TRIANGLE_C_POS)
{
	//点Pが三角形Aの外側の頂点領域の中にあるか確認--------------------------------------------

	float3 lAb = TRIANGLE_B_POS - TRIANGLE_A_POS;
	float3 lAc = TRIANGLE_C_POS - TRIANGLE_A_POS;
	float3 lAp = POINT_POS - TRIANGLE_A_POS;

	float lD1 = dot(lAb,lAp);
	float lD2 = dot(lAc,lAp);

	if (lD1 <= 0.0f && lD2 <= 0.0f)
	{
		return TRIANGLE_A_POS;
	}

	//点Pが三角形Aの外側の頂点領域の中にあるか確認--------------------------------------------



	//点Pが三角形Bの外側の頂点領域の中にあるか確認--------------------------------------------

	float3 lBp = POINT_POS - TRIANGLE_B_POS;
	float lD3 = dot(lAb,lBp);
	float lD4 = dot(lAc,lBp);
	if (0.0f <= lD3 && lD4 <= lD3)
	{
		return TRIANGLE_B_POS;
	}

	//点Pが三角形Bの外側の頂点領域の中にあるか確認--------------------------------------------


	//点PがABの辺領域の中にあるかどうか確認し、あればPのAB上に対する射影を返す--------------------------------------------

	float lVc = lD1 * lD4 - lD3 * lD2;
	if (lVc <= 0.0f && 0.0f <= lD1 && lD3 <= 0.0f)
	{
		float lV = lD1 / (lD1 - lD3);
		return TRIANGLE_A_POS + lAb * lV;
	}

	//点PがABの辺領域の中にあるかどうか確認し、あればPのAB上に対する射影を返す--------------------------------------------

	//点PがCの外側の頂点領域の中にあるかどうか確認--------------------------------------------

	float3 lCp = POINT_POS - TRIANGLE_C_POS;
	float lD5 = dot(lAb,lCp);
	float lD6 = dot(lAc,lCp);
	if (0.0f <= lD6 && lD5 <= lD6)
	{
		return TRIANGLE_C_POS;
	}
	//点PがCの外側の頂点領域の中にあるかどうか確認--------------------------------------------



	//点PがACの辺領域の中にあるかどうか確認し、あればPのAC上に対する射影を返す--------------------------------------------

	float lVb = lD5 * lD2 - lD1 * lD6;
	if (lVb <= 0.0f && 0.0f <= lD2 && lD6 <= 0.0f)
	{
		float lW = lD2 / (lD2 - lD6);
		return TRIANGLE_A_POS + lAc * lW;
	}

	//点PがACの辺領域の中にあるかどうか確認し、あればPのAC上に対する射影を返す--------------------------------------------


	//点PがBCの辺領域の中にあるかどうか確認し、あればPのBC上に対する射影を返す--------------------------------------------

	float lVa = lD3 * lD6 - lD5 * lD4;
	float lD4SubD3 = lD4 - lD3;
	float lD5SubD6 = lD5 - lD6;
	if (lVa <= 0.0f && 0.0f <= lD4SubD3 && 0.0f <= lD5SubD6)
	{
		float lW = lD4SubD3 / (lD4SubD3 + lD5SubD6);
		return TRIANGLE_B_POS + (TRIANGLE_C_POS - TRIANGLE_B_POS) * lW;
	}

	//点PがBCの辺領域の中にあるかどうか確認し、あればPのBC上に対する射影を返す--------------------------------------------


	//点Pは面領域の中にある。Qをその重心座標(u,v,w)を用いて計算--------------------------------------------
	float lDenom = 1.0f / (lVa + lVb + lVc);
	float lV = lVb * lDenom;
	float lW = lVc * lDenom;
	return TRIANGLE_A_POS + lAb * lV + lAc * lW;;
}




cbuffer RootConstants : register(b0)
{
    matrix scaleRotateBillboardMat;
    matrix viewProjection;
    float4 pos;
    float radius;
    uint meshNum;
	uint hitVel;
};

struct HitBoxData
{
    float3 trianglePos[3];
    float3 normal;
};

//三角形の情報
RWStructuredBuffer<HitBoxData> hitBoxData : register(u0);

struct OutputData
{
    matrix mat;
    float4 color;
};
RWStructuredBuffer<OutputData> outputData : register(u1);

//パーティクル座標
struct ParticleData
{
	float4 pos;
	float4 color;
};
RWStructuredBuffer<ParticleData> particleData : register(u2);
//基本座標
RWStructuredBuffer<ParticleData> baseParticleData : register(u3);

[numthreads(1024, 1, 1)]
void CSmain(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex,uint3 groupThreadID : SV_GroupThreadID)
{
    uint index = groupThreadID.x;
    index += 1024 * groupId.x;

	float4 particlePos = particleData[index].pos;

	bool hitFlag = false;	
    float4 hitColor;

    for(int i = 0;i < meshNum; ++i)
    {
        //最近接点の算出
        float3 pointPos = ClosestPoint(particlePos.xyz, hitBoxData[i].trianglePos[0], hitBoxData[i].trianglePos[1], hitBoxData[i].trianglePos[2]);
        float distanceResult = distance(pointPos,particlePos.xyz);

        //当たり判定
        if(distanceResult <= radius)
        {		
			particleData[index].pos.xyz += (hitBoxData[i].normal * hitVel);
            hitColor.xyz = hitBoxData[i].normal;			
			hitFlag = true;
        }
        else
        {
            hitColor = float4(0.6,0.6,0.6,1);
        }
		if(hitFlag)
		{
			break;
		}
    }

	//if(hitFlag)
	{
		particleData[index].color = hitColor;
	}
}