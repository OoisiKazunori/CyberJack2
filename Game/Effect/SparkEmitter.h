#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Effect/SparkParticle.h"

class SparkEmitter
{
public:
	SparkEmitter();

	void Init(const KazMath::Vec3<float> *POS);
	void Finalize();
	void Update();
	void Draw();

private:
	static const int PARTICLE_MAX_NUM = 100;
	std::array<SparkParticle, PARTICLE_MAX_NUM> particle;

	const KazMath::Vec3<float> *pos;
	bool initFlag;
	int timer;
	int particleIndex;
};
