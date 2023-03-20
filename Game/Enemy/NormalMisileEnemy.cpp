#include "NormalMisileEnemy.h"
#include"../KazLibrary/Loader/ObjResourceMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Math/KazMath.h"
#include"../KazLibrary/Easing/easing.h"

NormalMisileEnemy::NormalMisileEnemy()
{
	flashR.data.colorData.color = { 0,255,0,255 };
	circleFlashR.data.colorData.color = { 0,255,0,255 };

	flashR.data.radius = 10.0f;
	circleFlashR.data.radius = 10.0f;
	flashR.data.change3DFlag = true;
	circleFlashR.data.change3DFlag = true;
	iEnemy_EnemyStatusData->radius = 3.0f;
}

void NormalMisileEnemy::Init(const EnemyGenerateData &GENERATE_DATA, bool DEMO_FLAG)
{
	lerpPos = GENERATE_DATA.initPos;	//座標の初期化

	EnemyModelType lModelType = ENEMY_MODEL_FBX;
	if (GENERATE_DATA.useMeshPaticleFlag)
	{
		lModelType = ENEMY_MODEL_MESHPARTICLE;
	}
	InitModel(KazMath::Transform3D(GENERATE_DATA.initPos, { 1.0f,1.0f,1.0f }, { 0.0f,180.0f,0.0f }), KazFilePathName::EnemyPath + "MisileEnemy/" + "Gunner_Switch_anim.fbx", 10.0f, lModelType);
	iOperationData.Init(1,"gw-M");							//残りロックオン数等の初期化

	initShotFlag = false;

	initDeadSoundFlag = false;
	demoFlag = DEMO_FLAG;

	shotTimer = 0;

	disappearTimer = 0;
	flashTimer = 0;
	startFlag = false;

	shotFlag = GENERATE_DATA.misileEnemy.isShotFlag;

	iEnemy_EnemyStatusData->startFlag = true;
	iEnemy_EnemyStatusData->radius = 10.0f;
}

void NormalMisileEnemy::Finalize()
{
}

void NormalMisileEnemy::Update()
{
	if (shotFlag)
	{
		//発射
		//ゲーム内で攻撃モーションの起動
		bool lGameShotFlag = 120 <= shotTimer && !initShotFlag && iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag;

		if (!demoFlag && 120 - 60 <= shotTimer && !initShotFlag && iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag)
		{
			startFlag = true;
		}

		if (90 <= shotTimer)
		{
			iEnemy_FbxModelRender->data.isPlayFlag = true;
		}

		if (lGameShotFlag)
		{
			iEnemy_EnemyStatusData->genarateData.enemyGenerateData.initPos = iEnemy_FbxModelRender->data.transform.pos;
			iEnemy_EnemyStatusData->genarateData.enemyType = ENEMY_TYPE_MISILE_SPLINE;

			//ノックバック
			KazMath::Vec3<float> kockBackVel = { 0.0f,0.0f,60.0f };
			lerpPos = iEnemy_FbxModelRender->data.transform.pos + kockBackVel;
			//ShotSound();
			initShotFlag = true;
			debugShotFlag = false;
			startFlag = true;
		}

		if (initShotFlag)
		{
			iEnemy_FbxModelRender->data.transform.rotation.z += 5.0f;
		}
	}
	else
	{
		iEnemy_FbxModelRender->data.transform.rotation.z += 5.0f;
	}


	//死亡演出処理
	//デバックキーor当たり判定内&&死亡時
	if (!ProcessingOfDeathFbx(DEATH_ROLL))
	{
		//登場処理
		if (iEnemy_FbxModelRender->data.colorData.color.a < 255)
		{
			iEnemy_FbxModelRender->data.colorData.color.a += 5;
		}
		else
		{
			iEnemy_FbxModelRender->data.colorData.color.a = 255;
		}

		if (!demoFlag || debugShotFlag)
		{
			++shotTimer;
		}

		KazMath::Vec3<float> vel = { 0.0f,0.0f,-1.0f };
		if (!demoFlag)
		{
			lerpPos += vel;
		}
	}

	KazMath::Larp(lerpPos.z, &iEnemy_FbxModelRender->data.transform.pos.z, 0.1f);


	if (!startFlag)
	{
		disappearTimer = 0;
		flashTimer = 0;
		circleFlashTimer = 0;
	}

	circleFlashR.data.transform.pos = iEnemy_FbxModelRender->data.transform.pos + KazMath::Vec3<float>(0.0f, 0.0f, -10.0f);
	flashR.data.transform.pos = iEnemy_FbxModelRender->data.transform.pos;

	int lT = 0;
	bool flashFlag = false;
	if (15 <= flashTimer)
	{
		++disappearTimer;
		lT = disappearTimer;
		flashTimer = 15;
		flashFlag = true;
	}
	else
	{
		++flashTimer;
		lT = flashTimer;
	}
	++circleFlashTimer;

	if (10 <= disappearTimer)
	{
		disappearTimer = 10;
	}



	float lScale = 0.0f;
	if (flashTimer <= 1)
	{
		circleFlashR.data.transform.scale.x = 0.1f;
		circleFlashR.data.transform.scale.y = 0.1f;
		circleFlashR.data.transform.scale.z = 0.1f;
	}
	else if (flashTimer <= 2)
	{
		circleFlashR.data.transform.scale.x = 1.5f;
		circleFlashR.data.transform.scale.y = 1.0f;
		circleFlashR.data.transform.scale.z = 1.0f;
		lScale = 0.15f;
	}
	else
	{
		circleFlashR.data.transform.scale.x = 1.0f + -EasingMaker(Out, Cubic, KazMath::ConvertTimerToRate(circleFlashTimer, 40)) * 1.0f;
		circleFlashR.data.transform.scale.y = 1.0f + -EasingMaker(Out, Cubic, KazMath::ConvertTimerToRate(circleFlashTimer, 40)) * 1.0f;
		circleFlashR.data.transform.scale.z = 1.0f + -EasingMaker(Out, Cubic, KazMath::ConvertTimerToRate(circleFlashTimer, 40)) * 1.0f;
		lScale = 0.15f;
	}
	flashR.data.transform.scale.y = lScale + (-EasingMaker(Out, Cubic, KazMath::ConvertTimerToRate(disappearTimer, 10)) * lScale);

	if (iEnemy_FbxModelRender->data.transform.pos.z <= -50.0f)
	{
		iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag = false;
		iEnemy_EnemyStatusData->outOfStageFlag = true;
	}
}

void NormalMisileEnemy::Draw()
{
	LockOnWindow(iEnemy_FbxModelRender->data.transform.pos);

	if (iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag)
	{
		flashR.Draw();
		circleFlashR.Draw();
	}
	iEnemy_FbxModelRender->Draw();
}
