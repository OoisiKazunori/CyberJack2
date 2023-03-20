#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../KazLibrary/Easing/easing.h"
#include"../Game/Effect/EaseScaleParticle.h"
#include"../Game/Effect/IHitEffectEmitter.h"

class HitEffectPattern2Emitter :public IHitEffectEmitter
{
public:
	HitEffectPattern2Emitter();

	void Init(const KazMath::Vec2<float> &POS);
	void Update();
	void Draw();

	bool IsActive();
private:
	KazMath::Vec2<float>baseScale;
	KazMath::Vec2<float>easeScale;
	KazMath::Vec2<int>timer;
	std::unique_ptr<EaseScaleParticle> particle;
};

