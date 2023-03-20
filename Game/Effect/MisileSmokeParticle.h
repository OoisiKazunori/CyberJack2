#pragma once
#include"../KazLibrary/Render/KazRender.h"

class MisileSmokeParticle
{
public:
	MisileSmokeParticle();
	void Init(const KazMath::Vec3<float> &EMITT_POS);
	void Finalize();
	void Update();
	void Draw();

	bool IsAlive();

private:
	CircleRenderPtr patricle;
	bool initFlag;
};

