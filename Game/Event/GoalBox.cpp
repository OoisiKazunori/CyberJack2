#include "GoalBox.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Imgui/MyImgui.h"

GoalBox::GoalBox()
{
	model = std::make_unique<ObjModelRender>();
	model->data.pipelineName = PIPELINE_NAME_OBJ_MULTITEX;
	iOperationData.Init(8,"Network_Security");
	prevHpNum = iOperationData.rockOnNum;

	model->data.handle = ObjResourceMgr::Instance()->LoadModel(KazFilePathName::GoalPath + "goal.obj");

	lerpPos = { 0.0f,0.0f,0.0f };
	lerpRota = { 0.0f,0.0f,0.0f };
	model->data.transform.pos = lerpPos;
	model->data.transform.rotation = lerpRota;

	//model->data.transform.scale = { 10.0f,10.0f,10.0f };

	lerpMoveRotaVel = 0.1f;
	lerpMoveVel = 0.1f;
	moveRotaVel = { 200.0f,200.0f,200.0f };
	moveVel = { 0.0f,0.0f,15.0f };

	goCenterPos = { 0.0f, 3.0f, 50.0f };
	lightEffect.Init(&model->data.motherMat.mat);

	hitBox.center = &model->data.transform.pos;
	hitBox.radius = 10.0f;
	//std::shared_ptr<DirectX::XMMATRIX>data = std::make_shared<DirectX::XMMATRIX>(model->data.motherMat);

	damageSoundHandle = SoundManager::Instance()->LoadSoundMem(KazFilePathName::SoundPath + "EenemyDamage.wav", false);
	warpSoundHandle = SoundManager::Instance()->LoadSoundMem(KazFilePathName::SoundPath + "warp.wav", false);
}

void GoalBox::Init(const KazMath::Vec3<float> &POS)
{
	iOperationData.Init(HP, "Network_Security");
	prevHpNum = iOperationData.rockOnNum;


	lerpPos = POS;
	lerpRota = { 0.0f,0.0f,0.0f };
	lerpScale = { 1.0f,1.0f,1.0f };

	model->data.transform.pos = lerpPos;
	model->data.transform.rotation = lerpRota;

	lightEffect.Init(model->GetMotherMatrixPtr());
	addRota = 0;

	startPortalEffectFlag = false;
	addVel = 0.0f;

	intervalTimer = 0;
	initWarpSoundFlag = false;
}

void GoalBox::Update()
{
	if (KeyBoradInputManager::Instance()->InputTrigger(DIK_M))
	{
		Hit();
	}
	if (KeyBoradInputManager::Instance()->InputTrigger(DIK_N))
	{
		Init({ 20.0f,10.0f,10.0f });
	}

	bool reduceHpFlag = false;

	//リリース時に残りロックオン数が前と違ったら当たった判定を出す
	if (releaseFlag && iOperationData.rockOnNum != prevHpNum)
	{
		hitFlag = true;
		prevHpNum = iOperationData.rockOnNum;
	}

	//当たった判定を出て、Hpが指定の回数分減らしたら酋長
	if (hitFlag)
	{
		int result = ReduceHp(HP - iOperationData.rockOnNum);
		//体力を減らす
		if (result == 1)
		{
			SoundManager::Instance()->PlaySoundMem(damageSoundHandle, 1);
			reduceHpFlag = true;
		}
		//終了
		else if (result == 2)
		{
			hitFlag = false;
		}
	}

	//常に回転する
	lerpRota.x += 1.0f;
	lerpRota.y += 1.0f;

	//当たった際の挙動
	if (reduceHpFlag)
	{
		//後ろに飛ぶ
		lerpPos.z = model->data.transform.pos.z + moveVel.z;
		//回転させる
		lerpRota = model->data.transform.rotation + moveRotaVel;
		lightEffect.Appear();
	}

	//全弾ヒットしたら演出消して画面中央に向かう
	const int NO_HP = 0;
	if (iOperationData.hp <= NO_HP)
	{
		++intervalTimer;
	}

	const int INTERVAL_MAX_TIMER = 30;
	if (INTERVAL_MAX_TIMER <= intervalTimer)
	{
		lightEffect.Disappear();
		lerpPos = goCenterPos;

		if (120 <= intervalTimer)
		{
			addRota += addVel;
			addVel += 0.1f;
		}
		lerpRota.x = 0.0f;
		lerpRota.y += addRota;
		lerpRota.z = 0.0f;


		if (60.0f * 5.0f <= addRota)
		{
			lerpScale = { 35.0f ,35.0f,35.0f };
		}
		if (60.0f * 10.0f <= addRota)
		{
			KazMath::Vec3<float> adj = { 0.0f,0.0f,-15.0f };
			startPortalEffectFlag = true;
		}
	}


	//座標のラープ
	{
		KazMath::Vec3<float> distance = lerpPos - model->data.transform.pos;
		model->data.transform.pos += distance * lerpMoveVel;
	}
	//回転のラープ
	{
		KazMath::Vec3<float> distance = lerpRota - model->data.transform.rotation;
		model->data.transform.rotation += distance * lerpMoveRotaVel;
	}

	{
		KazMath::Vec3<float> distance = lerpScale - model->data.transform.scale;
		model->data.transform.scale += distance * 0.1f;
	}

	lightEffect.Update();
}

void GoalBox::Draw()
{
	model->Draw();
	LockOnWindow(model->data.transform.pos);
}

void GoalBox::Appear(const KazMath::Vec3<float> &POS)
{
	lerpPos = POS;
}