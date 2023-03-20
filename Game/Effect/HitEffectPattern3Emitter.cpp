#include "HitEffectPattern3Emitter.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

HitEffectPattern3Emitter::HitEffectPattern3Emitter()
{
	for (int i = 0; i < particle.size(); ++i)
	{
		particle[i] = std::make_unique<EaseScaleParticle>(TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::RelativeResourcePath + "Effect/Smoke/smoke10.png"));
	}


	float lSameScale = 0.5f;
	baseScale[0] = { lSameScale,lSameScale };
	baseScale[1] = { lSameScale,lSameScale };
	baseScale[2] = { lSameScale,lSameScale };
	baseScale[3] = { lSameScale,lSameScale };

	easeScale[0] = { lSameScale,lSameScale };
	easeScale[1] = { lSameScale,lSameScale };
	easeScale[2] = { lSameScale,lSameScale };
	easeScale[3] = { lSameScale,lSameScale };

	timer[0] = { 60,60 };
	timer[1] = { 60,60 };
	timer[2] = { 60,60 };
	timer[3] = { 60,60 };

	initFlag = false;
	particleDrawIndex = 0;
}

void HitEffectPattern3Emitter::Init(const KazMath::Vec2<float> &POS)
{
	for (int i = 0; i < PARTICLE_MAX - 1; ++i)
	{
		particle[i]->Init(POS + KazMath::Vec2<float>(0.0f, static_cast<float>(i) * 100.0f), baseScale[i], easeScale[i], timer[i], true);
	}

	particle[PARTICLE_MAX - 1]->Init(POS + KazMath::Vec2<float>(-10.0f, 200.0f), KazMath::Vec2<float>(0.4f, 0.1f), KazMath::Vec2<float>(1.3f, 1.3f), KazMath::Vec2<int>(60, 20), false);

	particleDrawIndex = 0;
	flame = 0;
	initFlag = true;
}

void HitEffectPattern3Emitter::Update()
{
	for (int i = 0; i < particle.size(); ++i)
	{
		particle[i]->Update();
	}

	if (initFlag)
	{
		++flame;
		if (flame % 2 == 0)
		{
			++particleDrawIndex;
		}

		if (particle.size() <= particleDrawIndex)
		{
			particleDrawIndex = static_cast<int>(particle.size());
		}
	}
	else
	{
		flame = 0;
		particleDrawIndex = 0;
	}
}

void HitEffectPattern3Emitter::Draw()
{
	particle[PARTICLE_MAX - 1]->Draw();
	for (int i = 0; i < particleDrawIndex; ++i)
	{
		particle[i]->Draw();
	}
}

bool HitEffectPattern3Emitter::IsActive()
{
	for (int i = 0; i < particle.size(); ++i)
	{
		if (particle[i]->IsAlive())
		{
			return true;
		}
	}
	return false;
}
