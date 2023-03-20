#include "SplineMisileForBattleShip.h"
#include"../KazLibrary/Math/KazMath.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

SplineMisileForBattleShip::SplineMisileForBattleShip()
{
	maxTime = KazMath::ConvertSecondToFlame(10);
	splineBox.data.color = { 255,0,0,255 };
}

void SplineMisileForBattleShip::Init(const EnemyGenerateData &GENERATE_DATA, bool DEMO_FLAG)
{
	InitModel(KazMath::Transform3D(GENERATE_DATA.initPos, { 0.4f,0.4f,0.4f }, { 0.0f,0.0f,0.0f }), KazFilePathName::EnemyPath + +"BattleShipMisile/" + "BattleshipMissile_Model.obj", 5.0f, ENEMY_MODEL_OBJ);

	iEnemy_EnemyStatusData->objFlag = true;
	iEnemy_EnemyStatusData->timer = maxTime;
	iOperationData.Init(1,"Kaz-M");

	startIndex = 1;
	timeRate = 0.0f;


	points.clear();
	points.shrink_to_fit();

	KazMath::Vec3<float> startPos = GENERATE_DATA.initPos;
	KazMath::Vec3<float> endPos = { 0.0f,0.0f,0.0f };


	KazMath::Vec3<float>upMaxPos = {};
	KazMath::Vec3<float>downMaxPos = {};
	KazMath::Vec3<float> moveToPlayerPos = {};
	
	switch (GENERATE_DATA.battleShipEnemyData.shotEnum)
	{
	case BATTLESHIP_MISILE_SHOT_RIGHT:
		upMaxPos = startPos + KazMath::Vec3<float>(20.0f, 100.0f, 0.0f);
		downMaxPos = upMaxPos + KazMath::Vec3<float>(10.0f, -20.0f, -30.0f);
		moveToPlayerPos = downMaxPos + KazMath::Vec3<float>(0.0f, -5.0f, -30.0f);
		break;
	case BATTLESHIP_MISILE_SHOT_LEFT:
		upMaxPos = startPos + KazMath::Vec3<float>(-20.0f, 100.0f, 0.0f);
		downMaxPos = upMaxPos + KazMath::Vec3<float>(-10.0f, -20.0f, -30.0f);
		moveToPlayerPos = downMaxPos + KazMath::Vec3<float>(0.0f, -5.0f, -30.0f);
		break;
	default:
		break;
	}


	//スタート地点
	points.push_back(startPos);
	points.push_back(startPos);

	//中間
	points.push_back(upMaxPos);
	points.push_back(downMaxPos);
	points.push_back(moveToPlayerPos);

	//ゴール地点
	points.push_back(endPos);
	points.push_back(endPos);

	pointTime = maxTime / (static_cast<int>(points.size()) - 3);
	nowTime = 0;
	initDeadSoundFlag = false;
	hitFlag = false;

	demoFlag = DEMO_FLAG;

	rocketLight.Fisnish();

	//smokeEmitter.Init(&iEnemy_ModelRender->data.transform.pos);
	iEnemy_EnemyStatusData->radius = 0.0f;
	iEnemy_EnemyStatusData->startFlag = false;
	
}
void SplineMisileForBattleShip::Finalize()
{
	//smokeEmitter.Finalize();
}

void SplineMisileForBattleShip::Update()
{
	//当たったら描画しなくなる
	if (iEnemy_EnemyStatusData->timer <= 0)
	{
		iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag = false;
		hitFlag = true;
	}


	//死亡演出処理
	//デバックキーor当たり判定内&&死亡時
	if (!ProcessingOfDeath(DEATH_SINK) && !hitFlag)
	{
		if (iOperationData.enableToHitFlag)
		{
			--iEnemy_EnemyStatusData->timer;
		}

		//移動時間----------------------------------------------
		timeRate = static_cast<float>(nowTime) / static_cast<float>(pointTime);
		if (1.0f <= timeRate)
		{
			if (startIndex < points.size() - 3)
			{
				startIndex++;
				timeRate = 0.0f;
				nowTime = 0;
			}
			else
			{
				timeRate = 1.0f;
			}
		}
		++nowTime;
		//移動時間----------------------------------------------


		//座標設定----------------------------------------------
		position = KazMath::SplinePosition(points, static_cast<size_t>(startIndex), timeRate, true);

		for (int i = 0; i < pointsRender.size(); ++i)
		{
			pointsRender[i].data.transform.pos = points[i];
		}

		iEnemy_ObjModelRender->data.transform.pos = position;
		//座標設定----------------------------------------------

		//前ベクトルの設定----------------------------------------------
		float nextTimeRate = static_cast<float>(nowTime + 1) / static_cast<float>(pointTime);
		KazMath::Vec3<float> nextpos = KazMath::SplinePosition(points, startIndex, nextTimeRate, true);


		if (startIndex != 2)
		{
			KazMath::Vec3<float> upVector = nextpos - position;
			upVector.Normalize();
			iEnemy_ObjModelRender->data.upVector = upVector;
		}
		//炎点火
		if (startIndex == 2)
		{
			//rocketLight.Init(&iEnemy_ModelRender->data.transform.pos, KazMath::Vec3<float>(0.0f, 0.0f, -5.0f), false);
			//rocketLight.ChangeLightRadius(3.0f);
			iEnemy_EnemyStatusData->radius = 3.0f;
			iEnemy_EnemyStatusData->startFlag = true;
		}
		//炎縮小
		if (3 <= startIndex)
		{
			//rocketLight.ChangeLightRadius(0.1f);
			iEnemy_EnemyStatusData->radius = 0.5f;
		}
		//前ベクトルの設定----------------------------------------------
	}

	//死亡処理
	if (iEnemy_ObjModelRender->data.colorData.color.a <= 0.0f)
	{
		iOperationData.enableToHitFlag = false;
	}

	//rocketLight.Update();
	//smokeEmitter.Update();
}

void SplineMisileForBattleShip::Draw()
{
	if (iOperationData.enableToHitFlag)
	{
		if (demoFlag)
		{
			for (int i = 0; i < pointsRender.size(); ++i)
			{
				pointsRender[i].Draw();
			}
		}
		iEnemy_ObjModelRender->Draw();
		LockOnWindow(iEnemy_ObjModelRender->data.transform.pos);
	}
}
