#pragma once
#include"../KazLibrary/Render/KazRender.h"

class RocketLightEffect
{
public:
	RocketLightEffect();

	void Init(KazMath::Vec3<float> *BASE_POS_PTR, const KazMath::Vec3<float> &TEX_POS, bool APPEAR_FLAG, bool *IS_DEAD_FLAG, float *RADIUS, bool *START_FLAG);
	void Update();
	void Draw();

	bool IsStart();
	void Fisnish();
private:
	KazMath::Vec3<float> *pos;

	KazMath::Vec3<float>bloomTexPos;
	CircleRender lightEffectRender;

	int flashTimer;
	bool *initFlag;
	float *radius;
	bool *deadFlag;
};

