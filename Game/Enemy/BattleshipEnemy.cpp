#include "BattleshipEnemy.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

BattleshipEnemy::BattleshipEnemy()
{
	for (int i = 0; i < misileR.size(); ++i)
	{
		misileR[i].data.transform.pos.x = 5.0f;
		misileR[i].data.transform.pos.y = 88.0f;
		misileR[i].data.transform.pos.z = 25.0f;
	}

	topModel = std::make_unique<FbxModelRender>();

	//iEnemy_ModelRender = std::make_unique<FbxModelRender>();
	FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::EnemyPath + "BattleShip/" + "BattleshipEnemy_Head_anim.fbx");
	FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::EnemyPath + "BattleShip/" + "BattleshipEnemy_HachOpen_anim.fbx");

}

void BattleshipEnemy::Init(const EnemyGenerateData &GENERATE_DATA, bool DEMO_FLAG)
{
	InitModel(KazMath::Transform3D(GENERATE_DATA.initPos, { 0.5f,0.5f,0.5f }, { 0.0f,0.0f,0.0f }), KazFilePathName::EnemyPath + "BattleShip/" + "BattleshipEnemy_HachOpen_anim.fbx", 30.0f, ENEMY_MODEL_FBX);

	topModel->data.pipelineName = PIPELINE_NAME_FBX_RENDERTARGET_TWO_LIGHT;
	topModel->data.handle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::EnemyPath + "BattleShip/" + "BattleshipEnemy_Head_anim.fbx");	//モデル読み込み

	iEnemy_EnemyStatusData->meshParticleData.emplace_back(
		std::make_shared<MeshData>
		(
			topModel->data.handle.handle,
			&topModel->baseMatWorldData.matWorld
		)
	);

	iOperationData.Init(3, "Kaz-BS");							//残りロックオン数等の初期化
	initDeadSoundFlag = false;
	demoFlag = DEMO_FLAG;

	iEnemy_FbxModelRender->data.transform.pos.z = -1000.0f;
	basePos = GENERATE_DATA.initPos;
	vel = 0.0f;
	appearTimer = 0;
	misileShotIndex = 0;

	isShotFlag = GENERATE_DATA.battleShipData.isShotFlag;
	if (isShotFlag)
	{
		disappearTime = 15;
	}
	else
	{
		disappearTime = 10;
	}

	adjHitBoxPos = { 0.0f,95.0f,-15.0f };


	if (signbit(iEnemy_FbxModelRender->data.transform.pos.x))
	{
		topRota.y = 90.0f;
	}
	else if (!signbit(iEnemy_FbxModelRender->data.transform.pos.x))
	{
		topRota.y = -90.0f;
	}
	if (iEnemy_FbxModelRender->data.transform.pos.x == 0.0f)
	{
		topRota.y = 180.0f;
	}

	topPos = { 0.0f,82.0f,-16.0f };
}

void BattleshipEnemy::Finalize()
{
}

void BattleshipEnemy::Update()
{

	//ImGui::Begin("Dir");
	//KazImGuiHelper::InputVec3("LightDir", &lightDir);
	//ImGui::End();
	//SetLight(lightDir);

	hitBoxPos = {};
	hitBoxPos = iEnemy_FbxModelRender->data.transform.pos + adjHitBoxPos;
	iEnemy_EnemyStatusData->hitBox.center = &hitBoxPos;

	if (iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag)
	{
		//一定時間止まったらそのまま画面外に向かう
		if (KazMath::ConvertSecondToFlame(disappearTime) <= appearTimer && !demoFlag)
		{
			iEnemy_FbxModelRender->data.transform.pos.z += vel;
			vel += 0.1f;

			const float MAX_SPEED = 10.0f;
			if (MAX_SPEED <= vel)
			{
				vel = MAX_SPEED;

				if (0 < iEnemy_FbxModelRender->data.colorData.color.a)
				{
					iEnemy_FbxModelRender->data.colorData.color.a -= 5;
				}
				else
				{
					//死亡扱い
					iEnemy_FbxModelRender->data.colorData.color.a = 0;
					iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag = false;
					initDeadSoundFlag = true;
				}
			}
		}
		//ミサイル発射,目的地にたどり着く
		else if (isShotFlag && iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag)
		{

			int lBaseTimer = KazMath::ConvertSecondToFlame(8);
			int retuIndex = 0;
			int countIndex = 0;
			for (int i = 0; i < misileR.size(); ++i)
			{
				//ミサイル発射地点の設定--------------------------------------
				bool minusFlag = i % 2 != 0;
				if (2 <= countIndex)
				{
					countIndex = 1;
					++retuIndex;
				}
				else
				{
					++countIndex;
				}

				if (minusFlag)
				{
					misileR[i].data.transform.pos = iEnemy_FbxModelRender->data.transform.pos + KazMath::Vec3<float>(-5.0f, 88.0f, 25.0f) + KazMath::Vec3<float>(0.0f, 0.0f, 10.0f * retuIndex);
				}
				else
				{
					misileR[i].data.transform.pos = iEnemy_FbxModelRender->data.transform.pos + KazMath::Vec3<float>(5.0f, 88.0f, 25.0f) + KazMath::Vec3<float>(0.0f, 0.0f, 10.0f * retuIndex);
				}
				//ミサイル発射地点の設定--------------------------------------

				if (isShotFlag && !iEnemy_FbxModelRender->data.isPlayFlag && !iEnemy_FbxModelRender->data.isReverseFlag && lBaseTimer - KazMath::ConvertSecondToFlame(3) <= appearTimer)
				{
					iEnemy_FbxModelRender->data.isPlayFlag = true;
				}
				if (isShotFlag && !iEnemy_FbxModelRender->data.isReverseFlag && KazMath::ConvertSecondToFlame(11) <= appearTimer)
				{
					iEnemy_FbxModelRender->data.isReverseFlag = true;
					iEnemy_FbxModelRender->data.isPlayFlag = false;
				}

				//ミサイル発射
				if (appearTimer == lBaseTimer + i * 10)
				{
					iEnemy_EnemyStatusData->genarateData.enemyGenerateData.initPos = misileR[misileShotIndex].data.transform.pos;
					iEnemy_EnemyStatusData->genarateData.enemyType = ENEMY_TYPE_BATTLESHIP_MISILE;
					if (i % 2 == 0)
					{
						iEnemy_EnemyStatusData->genarateData.enemyGenerateData.battleShipEnemyData.shotEnum = BATTLESHIP_MISILE_SHOT_RIGHT;
					}
					else
					{
						iEnemy_EnemyStatusData->genarateData.enemyGenerateData.battleShipEnemyData.shotEnum = BATTLESHIP_MISILE_SHOT_LEFT;
					}
					++misileShotIndex;
					break;
				}
			}
		}


		if (appearTimer < KazMath::ConvertSecondToFlame(disappearTime))
		{
			KazMath::Larp(basePos.z, &iEnemy_FbxModelRender->data.transform.pos.z, 0.01f);

			//登場処理
			if (iEnemy_FbxModelRender->data.colorData.color.a < 255)
			{
				iEnemy_FbxModelRender->data.colorData.color.a += 5;
			}
			else
			{
				iEnemy_FbxModelRender->data.colorData.color.a = 255;
			}
		}
	}

	if (!ProcessingOfDeathFbx(DEATH_SINK))
	{
		++appearTimer;
	}
	else
	{
		topModel->data.pipelineName = PIPELINE_NAME_COLOR_WIREFLAME;
		topModel->data.removeMaterialFlag = true;
		topModel->data.colorData.color.x = 255;
		topModel->data.colorData.color.y = 255;
		topModel->data.colorData.color.z = 255;
	}

	topModel->data.transform = iEnemy_FbxModelRender->data.transform;
	topModel->data.colorData = iEnemy_FbxModelRender->data.colorData;
	topModel->data.transform.pos = iEnemy_FbxModelRender->data.transform.pos + topPos;
	topModel->data.transform.rotation = iEnemy_FbxModelRender->data.transform.rotation + topRota;
	//topModel->data.transform.scale += KazMath::Vec3<float>(0.15f, 0.15f, 0.15f);
}

void BattleshipEnemy::Draw()
{
	if (!iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag)
	{
		iEnemy_FbxModelRender->data.pipelineName = PIPELINE_NAME_COLOR_WIREFLAME;
	}

	iEnemy_FbxModelRender->Draw(!iEnemy_EnemyStatusData->meshParticleFlag);
	topModel->Draw(!iEnemy_EnemyStatusData->meshParticleFlag);



	LockOnWindow(*iEnemy_EnemyStatusData->hitBox.center);
}
