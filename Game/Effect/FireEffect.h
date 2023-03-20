#pragma once
#include"../KazLibrary/Render/KazRender.h"

class FireEffect
{
public:
	FireEffect();

	void Init(KazMath::Vec3<float> *BASE_POS_PTR, const KazMath::Vec3<float> &TEX_POS, bool APPEAR_FLAG, bool *IS_DEAD_FLAG);
	void Update();
	void Draw();

	bool IsStart();
	void Fisnish();

private:
	KazMath::Vec3<float> *pos;

	KazMath::Vec3<float>fireTexPos;
	Sprite3DRenderPtr fireEffectRender;

	bool initFlag;
	bool *deadFlag;
};

