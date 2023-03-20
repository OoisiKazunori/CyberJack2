#include "FireEffect.h"
#include "../KazLibrary/Helper/ResourceFilePass.h"

FireEffect::FireEffect()
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

void FireEffect::Init(KazMath::Vec3<float> *BASE_POS_PTR, const KazMath::Vec3<float> &TEX_POS, bool APPEAR_FLAG, bool *IS_DEAD_FLAG)
{
	fireTexPos = TEX_POS;
	pos = BASE_POS_PTR;
	if (APPEAR_FLAG)
	{
		fireEffectRender->data.colorData.color.a = 0;
	}
	else
	{
		fireEffectRender->data.colorData.color.a = 255;
	}
	initFlag = true;

	deadFlag = IS_DEAD_FLAG;
}

void FireEffect::Update()
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
		fireEffectRender->data.transform.pos = {};
		fireEffectRender->data.transform.pos = *pos + fireTexPos;


		if (!*deadFlag)
		{
			initFlag = false;
		}
	}
}

void FireEffect::Draw()
{
	if (initFlag)
	{
		fireEffectRender->Draw();
	}
}

bool FireEffect::IsStart()
{
    return initFlag;
}

void FireEffect::Fisnish()
{
	initFlag = false;
}
