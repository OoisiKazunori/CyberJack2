#include "HitEffectPattern2Emitter.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

HitEffectPattern2Emitter::HitEffectPattern2Emitter()
{
	particle = std::make_unique<EaseScaleParticle>(TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::RelativeResourcePath + "Effect/Smoke/smoke9.png"));


	baseScale = { 0.1f,0.1f };
	easeScale = { 0.8f,0.8f };
	timer = { 60,60 };
}

void HitEffectPattern2Emitter::Init(const KazMath::Vec2<float> &POS)
{
	particle->Init(POS, baseScale, easeScale, timer, false);
}

void HitEffectPattern2Emitter::Update()
{
	particle->Update();
}

void HitEffectPattern2Emitter::Draw()
{
	particle->Draw();
}

bool HitEffectPattern2Emitter::IsActive()
{
	if (particle->IsAlive())
	{
		return true;
	}
	return false;
}