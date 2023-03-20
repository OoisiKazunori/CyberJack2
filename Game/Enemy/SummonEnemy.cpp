#include "SummonEnemy.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Easing/easing.h"

SummonEnemy::SummonEnemy()
{
}

void SummonEnemy::Init(const EnemyGenerateData &GENERATE_DATA, bool DEMO_FLAG)
{
	basePos = GENERATE_DATA.initPos;
	iOperationData.Init(24, "snkr_BS");							//残りロックオン数等の初期化
	InitModel(KazMath::Transform3D(GENERATE_DATA.initPos + KazMath::Vec3<float>(0.0f, 0.0f, 30.0f), { 0.5f,0.5f,0.5f }, { 0.0f,180.0f,0.0f }), KazFilePathName::EnemyPath + "Summon/" + "SummonEnemy_anim.fbx", 15.0f, ENEMY_MODEL_FBX, true);

	initDeadSoundFlag = false;
	demoFlag = DEMO_FLAG;

	summonPos[0] = { -30.0f,-18.0f,0.0f };
	summonPos[1] = { 30.0f,-18.0f,0.0f };
	appearTimer = 0;
	disappearTime = 10;

	rotaMaxAngle = 15.0f;
}

void SummonEnemy::Finalize()
{
}

void SummonEnemy::Update()
{
	if(!ProcessingOfDeathFbx(DEATH_SINK))
	{
		++appearTimer;
		if (appearTimer < KazMath::ConvertSecondToFlame(2))
		{
			status = SummonEnemy::APPEAR;
		}
		else if (KazMath::ConvertSecondToFlame(2) < appearTimer && appearTimer < KazMath::ConvertSecondToFlame(disappearTime))
		{
			status = SummonEnemy::SUMMON;
		}
		else
		{
			status = SummonEnemy::DISAPPEAR;
		}

		if (KazMath::ConvertSecondToFlame(2) - 30 <= appearTimer && appearTimer < KazMath::ConvertSecondToFlame(disappearTime) - 30)
		{
			iEnemy_FbxModelRender->data.isPlayFlag = true;
		}
		if (KazMath::ConvertSecondToFlame(disappearTime) - 30 <= appearTimer)
		{
			iEnemy_FbxModelRender->data.isPlayFlag = false;
		}


		summonR[0].data.transform.pos = iEnemy_FbxModelRender->data.transform.pos + summonPos[0];
		summonR[1].data.transform.pos = iEnemy_FbxModelRender->data.transform.pos + summonPos[1];


		float disappearPosZ = basePos.z + 30.0f;
		float baseTimer = 120 + 120;
		switch (status)
		{
		case SummonEnemy::APPEAR:
			KazMath::Larp(basePos.z, &iEnemy_FbxModelRender->data.transform.pos.z, 0.01f);
			if (iEnemy_FbxModelRender->data.colorData.color.a < 255)
			{
				iEnemy_FbxModelRender->data.colorData.color.a += 5;
			}
			else
			{
				iEnemy_FbxModelRender->data.colorData.color.a = 255;
			}
			break;
		case SummonEnemy::SUMMON:

			for (int i = 0; i < 8; ++i)
			{
				if (appearTimer == baseTimer + i * 10)
				{
					iEnemy_EnemyStatusData->genarateData.enemyType = ENEMY_TYPE_POP;
					if (i % 2 == 0)
					{
						iEnemy_EnemyStatusData->genarateData.enemyGenerateData.initPos = summonR[0].data.transform.pos;
						iEnemy_EnemyStatusData->genarateData.enemyGenerateData.popEnemyData.leftRightFlag = true;
					}
					else
					{
						iEnemy_EnemyStatusData->genarateData.enemyGenerateData.initPos = summonR[1].data.transform.pos;
						iEnemy_EnemyStatusData->genarateData.enemyGenerateData.popEnemyData.leftRightFlag = false;
					}
					break;
				}
			}
			break;

		case SummonEnemy::DISAPPEAR:

			KazMath::Larp(disappearPosZ, &iEnemy_FbxModelRender->data.transform.pos.z, 0.01f);
			if (0 < iEnemy_FbxModelRender->data.colorData.color.a)
			{
				iEnemy_FbxModelRender->data.colorData.color.a += -5;
			}
			else
			{
				iEnemy_FbxModelRender->data.colorData.color.a = 0;
			}
			break;
		default:
			break;
		}
	}
}

void SummonEnemy::Draw()
{
	iEnemy_FbxModelRender->Draw(!iEnemy_EnemyStatusData->meshParticleFlag);
	LockOnWindow(iEnemy_FbxModelRender->data.transform.pos);
}
