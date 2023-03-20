#include "GoalBoxPortalEffect.h"
#include"../KazLibrary/Loader/TextureResourceMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Imgui/MyImgui.h"

GoalBoxPortalEffect::GoalBoxPortalEffect()
{
	noiseSprite = std::make_unique<Sprite3DRender>();
	constBuffHandle = noiseSprite->CreateConstBuffer(sizeof(float), typeid(float).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA);

	noiseSprite->data.pipelineName = PIPELINE_NAME_SPRITE_NOISE;
	portalSprite.data.pipelineName = PIPELINE_NAME_PORTAL;
	noiseSprite->data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TestPath + "Test.png");
}

void GoalBoxPortalEffect::Init(const KazMath::Vec3<float> &POS)
{
	startFlag = false;
	noiseSprite->data.transform.pos = POS;
	noiseSprite->data.transform.scale = { 0.18f,0.18f,0.0f };
	//lerpScale = noiseSprite->data.transform.scale;
	noiseSprite->data.transform.rotation = { 0.0f,0.0f,0.0f };
	timer = 0;
	appearNextStageFlag = false;
	changeStageFlag = false;
	drawPrevStageFlag = false;
	allHiddenCalledFlag = false;
	//noiseSprite->data.transform.scale.x = lerpScale.x;
}

void GoalBoxPortalEffect::Update()
{
	if (startFlag)
	{
		if (45 <= timer)
		{
			appearNextStageFlag = true;
		}

		float lSeed = static_cast<float>(timer);
		noiseSprite->TransData(&lSeed, constBuffHandle, typeid(float).name());



		if (appearNextStageFlag)
		{
			timer = -1;
		}
		else
		{
			++timer;
		}

		portalSprite.data.handleData = noiseSprite->data.handleData;
		portalSprite.data.transform = noiseSprite->data.transform;
	}
}

void GoalBoxPortalEffect::Draw()
{
	if (startFlag)
	{
		if (timer != -1)
		{
			noiseSprite->Draw();
		}
		else
		{
			portalSprite.Draw();
		}
	}
}

void GoalBoxPortalEffect::Start()
{
	startFlag = true;
}

bool GoalBoxPortalEffect::AllHidden()
{
	bool lPrevFlag = allHiddenCalledFlag;
	if (changeStageFlag)
	{
		allHiddenCalledFlag = true;
	}
	return changeStageFlag && !lPrevFlag;
}

bool GoalBoxPortalEffect::DrawPrevStageFlag()
{
	return drawPrevStageFlag;
}

void GoalBoxPortalEffect::CheckCameraPos(float POS_Z)
{
	if (noiseSprite->data.transform.pos.z <= POS_Z)
	{
		changeStageFlag = true;
		drawPrevStageFlag = true;
	}
}
