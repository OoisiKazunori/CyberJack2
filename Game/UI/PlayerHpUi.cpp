#include "PlayerHpUi.h"
#include"../KazLibrary/Loader/TextureResourceMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Math/KazMath.h"

PlayerHpUi::PlayerHpUi() :hpBackGround(KazFilePathName::HpPath + "HpBackGround.png")
{
	for (int i = 0; i < hpTex.size(); ++i)
	{
		hpTex[i] = TextureResourceMgr::Instance()->LoadGraphBuffer(KazFilePathName::HpPath + "Hp.png");
		hpFlame[i] = TextureResourceMgr::Instance()->LoadGraphBuffer(KazFilePathName::HpPath + "HpFlame.png");
		redHpTex[i] = TextureResourceMgr::Instance()->LoadGraphBuffer(KazFilePathName::HpPath + "RedHp.png");

		baseScale[i] = hpTex[i].m_transform.scale;
	}
	hpBackGround.m_transform.pos = { 200.0f,650.0f };

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
			lerpScale[i] = { 1.0f * baseScale[i].x,1.5f * baseScale[i].y };
		}
		else
		{
			lerpScale[i] = { 1.0f * baseScale[i].x,1.0f * baseScale[i].y };
		}
	}


	basePos = { 150.0f,650.0f };
	for (int i = 0; i < maxHp; ++i)
	{
		KazMath::Vec2<float> space = { i * 70.0f,0.0f };
		KazMath::Vec2<float> pos = basePos + space;
		hpTex[i].m_transform.pos = pos;
		hpFlame[i].m_transform.pos = pos;

		KazMath::Larp(lerpScale[i].y, &hpTex[i].m_transform.scale.y, 0.2f);


		redHpTex[i].m_transform = hpTex[i].m_transform;
	}

}

void PlayerHpUi::Draw(DrawingByRasterize& arg_rasterize)
{
	//Hp
	for (int i = 0; i < hp; ++i)
	{
		DrawFunc::DrawTextureIn2D(hpTex[i].m_drawCommand, hpTex[i].m_transform, hpTex[i].m_textureBuffer);
		arg_rasterize.ObjectRender(hpTex[i].m_drawCommand);
	}
	//RedHp
	for (int i = 0; i < redHpUiNum; ++i)
	{
		DrawFunc::DrawTextureIn2D(redHpTex[i].m_drawCommand, redHpTex[i].m_transform, redHpTex[i].m_textureBuffer);
		arg_rasterize.ObjectRender(redHpTex[i].m_drawCommand);
	}
	//Hp‚ÌƒtƒŒ[ƒ€
	for (int i = 0; i < maxHp; ++i)
	{
		DrawFunc::DrawTextureIn2D(hpFlame[i].m_drawCommand, hpFlame[i].m_transform, hpFlame[i].m_textureBuffer);
		arg_rasterize.ObjectRender(hpFlame[i].m_drawCommand);
	}
	DrawFunc::DrawTextureIn2D(hpBackGround.m_drawCommand, hpBackGround.m_transform, hpBackGround.m_textureBuffer);
	arg_rasterize.ObjectRender(hpBackGround.m_drawCommand);
}

void PlayerHpUi::Sub()
{
	--hp;
}
