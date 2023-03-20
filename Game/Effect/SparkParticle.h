#pragma once
#include"../KazLibrary/Render/KazRender.h"

class SparkParticle
{
public:
	SparkParticle();

	void Init(const KazMath::Vec3<float> &POS, int ANGLE);
	void Update();
	void Draw();

	bool IsActive();

private:
	std::array<KazMath::Vec3<float>, 2>vel;
	bool initFlag;

	std::array<Sprite3DRender, 2> particleR;
	int timer;
};

