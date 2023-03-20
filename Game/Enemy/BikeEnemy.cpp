#include "BikeEnemy.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

const float BikeEnemy::DISAPPEAR_Z_POS = 1000.0f;

BikeEnemy::BikeEnemy()
{
}

void BikeEnemy::Init(const EnemyGenerateData &GENERATE_DATA, bool DEMO_FLAG)
{
	float lScale = 0.7f;
	InitModel(KazMath::Transform3D(GENERATE_DATA.initPos, { lScale,lScale,lScale }, { 0.0f,0.0f,0.0f }), KazFilePathName::EnemyPath + "Bike/" + "BikeEnemy_anim.fbx", 15.0f, ENEMY_MODEL_FBX, false, true);
	iEnemy_FbxModelRender->data.colorData.color.a = 255;


	iOperationData.Init(2,"bt-V");							//残りロックオン数等の初期化
	initDeadSoundFlag = false;
	demoFlag = DEMO_FLAG;
	basePos = GENERATE_DATA.initPos;

	iEnemy_FbxModelRender->data.transform.pos.y += 7.0f;
	iEnemy_FbxModelRender->data.transform.pos.z = -1000.0f;
	line.data.startPos = GENERATE_DATA.initPos;
	line.data.endPos = GENERATE_DATA.initPos;
	line.data.startPos.z = -1000.0f;
	line.data.endPos.z = line.data.startPos.z;



	appearTimer = 0;
	emitt.Init(&sparkPos);
}

void BikeEnemy::Finalize()
{
}

void BikeEnemy::Update()
{
	float lPos = 0.0f;
	float lMul = 0.019f;

	sparkPos = iEnemy_FbxModelRender->data.transform.pos;
	sparkPos.y = line.data.startPos.y;
	sparkPos.z += -11.5f;

	if (!ProcessingOfDeathFbx(DEATH_SINK))
	{
		++appearTimer;

		/*
		登場
		線の登場後、敵を登場させる
		*/
		if (appearTimer <= 200)
		{
			lPos = basePos.z;
			kockBackPos.z = iEnemy_FbxModelRender->data.transform.pos.z - 20.0f;
			lMul = 0.05f;

			if (200 <= appearTimer)
			{
				iEnemy_FbxModelRender->data.isPlayFlag = true;
			}
		}
		/*
		発射
		*/
		else if (200 <= appearTimer && appearTimer <= 400)
		{
			//ミサイルの発射口を設定
			misileR.data.transform.pos.x = iEnemy_FbxModelRender->data.transform.pos.x + 5.0f;
			misileR.data.transform.pos.y = iEnemy_FbxModelRender->data.transform.pos.y + 5.0f;
			misileR.data.transform.pos.z = iEnemy_FbxModelRender->data.transform.pos.z + 14.0f;
			misileR2.data.transform.pos = misileR.data.transform.pos;
			misileR2.data.transform.pos.x -= 10.0f;

			//左右からミサイルを発射する--------------------------------------
			if (appearTimer == 310)
			{
				iEnemy_EnemyStatusData->genarateData.enemyGenerateData.initPos = misileR.data.transform.pos;
				iEnemy_EnemyStatusData->genarateData.enemyType = ENEMY_TYPE_BIKE_MISILE;
			}
			if (appearTimer == 311)
			{
				iEnemy_EnemyStatusData->genarateData.enemyGenerateData.initPos = misileR2.data.transform.pos;
				iEnemy_EnemyStatusData->genarateData.enemyType = ENEMY_TYPE_BIKE_MISILE;
			}
			if (appearTimer == 320)
			{
				iEnemy_FbxModelRender->data.isPlayFlag = false;
				iEnemy_FbxModelRender->data.isReverseFlag = true;
			}
			//左右からミサイルを発射する--------------------------------------

			lPos = basePos.z;
			kockBackPos.z = iEnemy_FbxModelRender->data.transform.pos.z - 20.0f;
		}
		/*
		退出準備
		敵を後ろに少しだけノックバックする
		*/
		else if (400 <= appearTimer && appearTimer <= 440)
		{
			lPos = kockBackPos.z;
			lMul = 0.1f;
		}
		/*
		退出
		線と敵を退出させる
		*/
		else
		{
			lPos = DISAPPEAR_Z_POS;
			KazMath::Larp(DISAPPEAR_Z_POS, &line.data.startPos.z, 0.02f);

			if (0 < iEnemy_FbxModelRender->data.colorData.color.a)
			{
				iEnemy_FbxModelRender->data.colorData.color.a += -5;
			}
			else
			{
				iEnemy_FbxModelRender->data.colorData.color.a = 0;
				iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag = false;
			}
		}

		KazMath::Larp(lPos, &iEnemy_FbxModelRender->data.transform.pos.z, lMul);
		//線を登場させる
		KazMath::Larp(DISAPPEAR_Z_POS, &line.data.endPos.z, 0.02f);

		misileR.data.color.color = { 255,0,0,255 };
		misileR2.data.color.color = { 255,0,0,255 };

		misileR.data.transform.pos.x = iEnemy_FbxModelRender->data.transform.pos.x + 5.0f;
		misileR.data.transform.pos.y = iEnemy_FbxModelRender->data.transform.pos.y + 5.0f;
		misileR.data.transform.pos.z = iEnemy_FbxModelRender->data.transform.pos.z + 14.0f;
		misileR2.data.transform.pos = misileR.data.transform.pos;
		misileR2.data.transform.pos.x -= 10.0f;

	}
	emitt.Update();
}

void BikeEnemy::Draw()
{
	
	if (!iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag)
	{
		iEnemy_FbxModelRender->data.pipelineName = PIPELINE_NAME_COLOR_WIREFLAME;
		iEnemy_FbxModelRender->data.removeMaterialFlag = true;
	}
	//misileR.Draw();
	//misileR2.Draw();
	iEnemy_FbxModelRender->Draw(!iEnemy_EnemyStatusData->meshParticleFlag);
	LockOnWindow(*iEnemy_EnemyStatusData->hitBox.center);

	if (iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag)
	{
		line.Draw();
		emitt.Draw();
	}
}
