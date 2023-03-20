#include "Player.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../KazLibrary/Sound/SoundManager.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

const int Player::COOL_MAX_TIME = 120;

Player::Player()
{
	hp = -1;
	pos = {};

	damageSoundHandle = SoundManager::Instance()->LoadSoundMem(KazFilePathName::SoundPath + "PlayerDamage.wav");

	fbxRender[LEFT].data.handle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::PlayerPath + "CH_Right_Back_Anim.fbx");
	fbxRender[RIGHT].data.handle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::PlayerPath + "CH_Left_Back_Anim.fbx");
	fbxRender[HEAD].data.handle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::PlayerPath + "CH_Model_Head.fbx");

	for (int i = 0; i < fbxRender.size(); ++i)
	{
		fbxRender[i].data.pipelineName = PIPELINE_NAME_FBX_RENDERTARGET_TWO;
		fbxRender[i].data.stopAnimationFlag = true;
		fbxRender[i].data.transform.scale = { 0.0f,0.0f,0.0f };
	}


	totalTime = FbxModelResourceMgr::Instance()->GetResourceData(fbxRender[0].data.handle.handle)->endTime[0] - FbxModelResourceMgr::Instance()->GetResourceData(fbxRender[0].data.handle.handle)->startTime[0];


	float lScale = 0.5f;
	minScale = { lScale ,lScale ,lScale };
	sinTimer = 0.0f;

	adjPos = { 0.0f,0.9f,0.3f };
}

void Player::Init(const KazMath::Vec3<float> &POS, bool DRAW_UI_FLAG, bool APPEAR_FLAG)
{
	pos = POS;
	if (APPEAR_FLAG)
	{
		for (int i = 0; i < fbxRender.size(); ++i)
		{
			fbxRender[i].data.colorData.color.a = 0;
			fbxRender[i].data.transform.scale = { 0.5f,0.5f,0.5f };
		}
	}
	else
	{
		for (int i = 0; i < fbxRender.size(); ++i)
		{
			fbxRender[i].data.colorData.color.a = 255;
			fbxRender[i].data.transform.scale = { 0.5f,0.5f,0.5f };
		}
	}
	hp = 3;

	hpUi.Init(hp);
	prevHp = hp;
	redFlag = false;
	coolTimer = 0;
	coolTimeFlag = false;

	drawHpFlag = DRAW_UI_FLAG;

	larpTime = 0;

}

void Player::Finalize()
{
	damageWindow.Finalize();
}

void Player::Input()
{
}

void Player::Update()
{
	hpUi.Update();


	//----------HPが減ったらプレイヤーを赤くする----------
	if (hp != prevHp)
	{
		SoundManager::Instance()->PlaySoundMem(damageSoundHandle, 1);
		damageEffect.Init(pos);
		damageWindow.Init(pos + KazMath::Vec3<float>(WIN_X / 2.0f, WIN_Y / 2.0f + 200.0f, 0.0f));
		redFlag = true;
	}
	prevHp = hp;

	if (redFlag)
	{
		++coolTimer;

		if (COOL_MAX_TIME <= coolTimer)
		{
			redFlag = false;
			coolTimeFlag = false;
		}
	}
	else
	{
		coolTimer = 0;
	}

	if (redFlag)
	{
		//render->data.color.color.x = 255;
		//render->data.color.color.y = 0;
		//render->data.color.color.z = 0;
	}
	else
	{
		//render->data.color.color.x = 255;
		//render->data.color.color.y = 255;
		//render->data.color.color.z = 255;
	}
	//----------HPが減ったらプレイヤーを赤くする----------

	damageEffect.Update();
	damageWindow.Update();

	for (int i = 0; i < fbxRender.size(); ++i)
	{
		fbxRender[i].data.colorData.color.a += 255 / 120;
		if (255 <= fbxRender[i].data.colorData.color.a)
		{
			fbxRender[i].data.colorData.color.a = 255;
		}
	}


	//ImGui::Begin("Head");
	//KazImGuiHelper::InputVec3("Pos", &adjPos);
	//KazImGuiHelper::InputVec3("Rota", &adjRota);
	//ImGui::End();

	fbxRender[HEAD].data.transform.rotation = adjRota;

	for (int i = 0; i < fbxRender.size() - 1; ++i)
	{
		fbxRender[i].data.transform.pos = pos + KazMath::Vec3<float>(0.0f, 1.0f + sinf(KazMath::PI_2F / 120.0f * sinTimer) * 0.2f, 0.0f);
	}

	fbxRender[HEAD].data.transform.pos = pos + adjPos + KazMath::Vec3<float>(0.0f, 1.0f + sinf(KazMath::PI_2F / 120.0f * sinTimer) * 0.2f, 0.0f);

	++sinTimer;
	++larpTime;
	if (30 <= larpTime)
	{
		float lScale = 0.55f;
		for (int i = 0; i < fbxRender.size(); ++i)
		{
			fbxRender[i].data.transform.scale = { lScale,lScale,lScale + 0.1f };
		}
		larpTime = 0;
	}

	for (int i = 0; i < fbxRender.size(); ++i)
	{
		KazMath::Larp(minScale, &fbxRender[i].data.transform.scale, 0.1f);
	}


	//プレイヤーのアニメーション制御
	float lNowRate = forceCameraRate + cameraRate.x * 0.5f;

	//左向き
	if (signbit(lNowRate))
	{
		leftFlag = true;
		rightFlag = false;
	}
	//右向き
	else
	{
		leftFlag = false;
		rightFlag = true;
	}


	if (leftFlag)
	{
		float lTotalTime = static_cast<float>(totalTime.Get());
		FbxLongLong convertTime = static_cast<FbxLongLong>(lTotalTime * -lNowRate);
		FbxTime lNowTime = convertTime;
		fbxRender[LEFT].currentTime = lNowTime;

		adjPos.x = 0.0f + -cameraRate.x * 0.2f;

		adjRota.x = 50.0f * -cameraRate.y;
		adjRota.y = -52.0f * -cameraRate.x;
		adjRota.z = 0.0f;
	}
	else if (rightFlag)
	{
		float lTotalTime = static_cast<float>(totalTime.Get());
		FbxLongLong convertTime = static_cast<FbxLongLong>(lTotalTime * lNowRate);
		FbxTime lNowTime = convertTime;
		fbxRender[RIGHT].currentTime = lNowTime;

		adjRota.x = 0.0f;
		adjRota.y = -270.0f + 60.0f * cameraRate.x;
		adjRota.z = 15.0f + 75.0f * -cameraRate.y;
	}

}

void Player::Draw()
{
	if (leftFlag)
	{
		fbxRender[LEFT].Draw();
	}
	else if (rightFlag)
	{
		fbxRender[RIGHT].Draw();
	}
	fbxRender[HEAD].Draw();

	damageEffect.Draw();
	damageWindow.Draw();

	if (drawHpFlag)
	{
		hpUi.Draw();
	}
}

void Player::Hit()
{
	if (!coolTimeFlag)
	{
		--hp;
		hpUi.Sub();
	}
	coolTimeFlag = true;
}

bool Player::IsAlive()
{
	return 0 < hp;
}
