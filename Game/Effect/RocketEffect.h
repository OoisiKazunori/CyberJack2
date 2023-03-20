#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Effect/RocketLightEffect.h"

class RocketEffect
{
public:
	RocketEffect();

	void Init(KazMath::Vec3<float> *BASE_POS_PTR, const KazMath::Vec3<float> &LIGHT_POS,const KazMath::Vec3<float> &FIRE_POS);
	void Update();
	void Draw();

private:
	KazMath::Vec3<float> *pos;

	KazMath::Vec3<float>fireTexPos;
	Sprite3DRenderPtr fireEffectRender;
	RocketLightEffect rocketLight;

	int flashTimer;
	bool initFlag;
};

