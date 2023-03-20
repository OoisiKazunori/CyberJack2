#include "EventSprite.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

EventSprite::EventSprite()
{
	hitBox.center = &spriteRender.data.transform.pos;
	hitBox.radius = 40.0f;

	selectHandle = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TitlePath + "ChoseStart.png");
	hitHandle = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TitlePath + "Start_Done.png");

}

void EventSprite::Init(const KazMath::Vec3<float> &POS, int HANDLE)
{
	noHitHandle = HANDLE;
	spriteRender.data.handleData = noHitHandle;
	spriteRender.data.transform.pos = POS;
	hitFlag = false;
	deadFlag = false;
	timer = 0;
	iOperationData.Init(1, "EventSprite");

	selectingFlag = false;

	spriteRender.data.transform.scale = { 0.5f,0.5f,1.0f };
	spriteRender.data.colorData.color.a = 255;
}

void EventSprite::Update()
{
	if (selectingFlag)
	{
		spriteRender.data.handleData = selectHandle;
	}
	else
	{
		spriteRender.data.handleData = noHitHandle;
	}
	if (!IsAlive())
	{
		hitFlag = true;
		spriteRender.data.handleData = hitHandle;
	}
	if (hitFlag)
	{
		++timer;
	}
	if (60 <= timer)
	{
		spriteRender.data.colorData.color.a -= 5;
	}

	if (spriteRender.data.colorData.color.a <= 0)
	{
		deadFlag = true;
	}

	spriteRender.data.transform.pos.y = 10.0f;
	spriteRender.data.transform.pos.z = 200.0f;
}

void EventSprite::Draw()
{
	if (!deadFlag)
	{
		spriteRender.Draw();
	}
}
