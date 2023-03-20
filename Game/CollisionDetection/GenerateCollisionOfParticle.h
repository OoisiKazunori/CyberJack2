#pragma once
#include"../KazLibrary/Helper/ResouceBufferHelper.h"

struct InitCollisionOfParticleData
{
	ResouceBufferHelper::BufferData hitBoxData;
	ResouceBufferHelper::BufferData particleData;
	ResouceBufferHelper::BufferData stackParticleHitBoxData;
	float particleRadius;
	float meshHitBoxRadius;
	UINT meshHitBoxNum;

	InitCollisionOfParticleData
	(
		const ResouceBufferHelper::BufferData &HIT_BOX_DATA,
		const ResouceBufferHelper::BufferData &PARTICLE_DATA,
		const ResouceBufferHelper::BufferData &STACK_PARTICLE_HIT_BOX_DATA,
		float PARTICLE_RADIUS,
		float MESH_HITBOX_RADIUS,
		UINT MESH_HIT_BOX_NUM
	):hitBoxData(HIT_BOX_DATA),
		particleData(PARTICLE_DATA),
		stackParticleHitBoxData(STACK_PARTICLE_HIT_BOX_DATA),
		particleRadius(PARTICLE_RADIUS),
		meshHitBoxNum(MESH_HIT_BOX_NUM),
		meshHitBoxRadius(MESH_HITBOX_RADIUS)
	{
	};
};

/// <summary>
/// メッシュ球判定とパーティクルを紐づける
/// </summary>
class GenerateCollisionOfParticle
{
public:
	GenerateCollisionOfParticle(
		const InitCollisionOfParticleData &INIT_DATA
	);
	void Compute();

private:
	ResouceBufferHelper computeHelper;
	struct ParticleHitBoxData
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		UINT meshID;
		DirectX::XMUINT3 id;
	};

	struct CommonData
	{
		float particleRadius;
		float meshHitBoxRadius;
		UINT meshHitBoxMaxNum;
	};

	static const int PARTICLE_MAX_NUM = 100000;
};