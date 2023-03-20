#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"MisileSmokeParticle.h"

class MisileSmokeEmitter
{
public:
	MisileSmokeEmitter();
	void Init(KazMath::Vec3<float> *EMITT_POS);
	void Finalize();
	void Update();
	void Draw();

private:
	std::array<MisileSmokeParticle, 50> smokeParticle;
	KazMath::Vec3<float>*pos;
	int intervalNum;
	int particleIndex;
	bool initFlag;
};

