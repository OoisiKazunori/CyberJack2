#include "RocketEffect.h"
#include "../KazLibrary/Helper/ResourceFilePass.h"

RocketEffect::RocketEffect()
{
	fireEffectRender = std::make_unique<Sprite3DRender>();
	fireEffectRender->data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::EffectFirePath + "Fire.png");
	fireEffectRender->data.billBoardFlag = false;

	fireTexPos = { 0.0f,0.0f,45.0f };
	fireEffectRender->data.pipelineName = PIPELINE_NAME_SPRITE_CUTALPHA;
	fireEffectRender->data.transform.scale = { 0.5f,1.5f,1.0f };
	fireEffectRender->data.transform.rotation = { 0.0f,90.0f,90.0f };
	initFlag = false;
}

void RocketEffect::Init(KazMath::Vec3<float> *BASE_POS_PTR, const KazMath::Vec3<float> &LIGHT_POS, const KazMath::Vec3<float> &FIRE_POS)
{
	rocketLight.Init(BASE_POS_PTR, LIGHT_POS, true, nullptr, nullptr, nullptr);
	fireTexPos = FIRE_POS;
	pos = BASE_POS_PTR;
	flashTimer = 0;
	fireEffectRender->data.colorData.color.a = 0;
	initFlag = true;
}

void RocketEffect::Update()
{
	if (initFlag)
	{
		if (fireEffectRender->data.colorData.color.a < 255)
		{
			fireEffectRender->data.colorData.color.a += 5;
		}
		else
		{
			fireEffectRender->data.colorData.color.a = 255;
		}


		if (3 <= flashTimer)
		{
			flashTimer = 0;
		}
		else
		{
			++flashTimer;
		}
		fireEffectRender->data.transform.pos = {};
		fireEffectRender->data.transform.pos = *pos + fireTexPos;


		rocketLight.Update();
	}
}

void RocketEffect::Draw()
{
	if (initFlag)
	{
		rocketLight.Draw();
		fireEffectRender->Draw();
	}
}
