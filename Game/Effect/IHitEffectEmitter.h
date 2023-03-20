#pragma once
#include"../KazLibrary/Math/KazMath.h"

class IHitEffectEmitter
{
public:
	virtual ~IHitEffectEmitter() {};

	virtual void Init(const KazMath::Vec2<float> &POS) = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	virtual bool IsActive() = 0;
};

