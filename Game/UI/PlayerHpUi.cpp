#include "PlayerHpUi.h"
#include"../KazLibrary/Loader/TextureResourceMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Math/KazMath.h"

PlayerHpUi::PlayerHpUi()
{
	RESOURCE_HANDLE hpHandle = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::HpPath + "Hp.png");
	RESOURCE_HANDLE hpFlameHandle = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::HpPath + "HpFlame.png");
	RESOURCE_HANDLE redHpHandle = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::HpPath + "RedHp.png");

	for (int i = 0; i < hpTex.size(); ++i)
	{
		hpTex[i].data.handleData = hpHandle;
		hpFlame[i].data.handleData = hpFlameHandle;
		redHpTex[i].data.handleData = redHpHandle;
	}

	hpBackGround.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::HpPath + "HpBackGround.png");
	hpBackGround.data.transform.pos = { 200.0f,650.0f };

}

void PlayerHpUi::Init(int HP_MAX)
{
	maxHp = HP_MAX;
	hp = maxHp;
	redHpUiNum = HP_MAX;
	prevHp = hp;
	disappearTimer = 0;

	timer[0] = 10;
	timer[1] = 5;
	timer[2] = 0;
	timer[3] = 15;
	timer[4] = 20;
	damageTimer = 0;
}

void PlayerHpUi::Update()
{
	if (hp <= 0)
	{
		hp = 0;
	}
	if (maxHp <= hp)
	{
		hp = maxHp;
	}


	if (hp != prevHp)
	{
		disappearFlag = true;
		disappearTimer = 0;
		subRedHpNum = prevHp - hp;
	}
	prevHp = hp;

	if (disappearFlag)
	{
		++disappearTimer;

		if (80 <= disappearTimer)
		{
			redHpUiNum -= subRedHpNum;
			subRedHpNum = 0;
			disappearFlag = false;
		}
	}

	if (redHpUiNum <= 0)
	{
		redHpUiNum = 0;
	}


	for (int i = 0; i < 5; ++i)
	{
		++timer[i];
		if (20 <= timer[i])
		{
			lerpFlag[i] = !lerpFlag[i];
			timer[i] = 0;
		}

		if (lerpFlag[i])
		{
			lerpScale[i] = { 1.0f,1.5f };
		}
		else
		{
			lerpScale[i] = { 1.0f,1.0f };
		}
	}


	basePos = { 150.0f,650.0f };
	for (int i = 0; i < maxHp; ++i)
	{
		KazMath::Vec2<float> space = { i * 70.0f,0.0f };
		KazMath::Vec2<float> pos = basePos + space;
		hpTex[i].data.transform.pos = pos;
		hpFlame[i].data.transform.pos = pos;

		KazMath::Larp(lerpScale[i].y, &hpTex[i].data.transform.scale.y, 0.2f);


		redHpTex[i].data.transform = hpTex[i].data.transform;
	}

}

void PlayerHpUi::Draw()
{
	//Hp
	for (int i = 0; i < hp; ++i)
	{
		hpTex[i].Draw();
	}
	//RedHp
	for (int i = 0; i < redHpUiNum; ++i)
	{
		redHpTex[i].Draw();
	}
	//Hp‚ÌƒtƒŒ[ƒ€
	for (int i = 0; i < maxHp; ++i)
	{
		hpFlame[i].Draw();
	}

	hpBackGround.Draw();
}

void PlayerHpUi::Sub()
{
	--hp;
}
