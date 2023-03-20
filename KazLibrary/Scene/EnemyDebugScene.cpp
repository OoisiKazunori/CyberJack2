#include "EnemyDebugScene.h"
#include"../Input/KeyBoradInputManager.h"
#include"../Input/ControllerInputManager.h"
#include"../Camera/CameraMgr.h"
#include"../KazLibrary/Loader/FbxModelResourceMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../Game/Enemy/BattleshipEnemy.h"
#include"../Game/Enemy/PopEnemy.h"
#include"../Game/Enemy/BikeEnemy.h"
#include"../Game/Enemy/SplineMisileForBikeEnemy.h"
#include"../Game/Enemy/SplineMisileForBattleShip.h"
#include"../Game/Enemy/SummonEnemy.h"

EnemyDebugScene::EnemyDebugScene()
{
	//uvCheckModel.data.handle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::TestPath + "boneTest.fbx");	//モデル読み込み



	battleShipFbxModel = std::make_unique<FbxModelRender>();
	//モデルの読み込み処理
	battleShipObjModel.data.handle = ObjResourceMgr::Instance()->LoadModel(KazFilePathName::EnemyPath + "BattleShip/" + "BattleshipEnemy_Model.obj");	//モデル読み込み
	//battleShipFbxModel->data.handle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::EnemyPath + "BattleShip/" + "BattleshipEnemy_HachOpen_anim.fbx");	//モデル読み込み


	battleShipObjModel.data.transform.pos = { -350.0f,0.0f,0.0f };
	battleShipFbxModel->data.transform.pos = { 200.0f,0.0f,0.0f };
	boxModel.data.transform.pos = { -200.0f,-50.0f,0.0f };
	boxModel.data.transform.scale = { 1.0f,1.0f,1.0f };


	//battleShipFbxModel->data.isPlayFlag = true;
	CameraMgr::Instance()->CameraSetting(60.0f, 10000.0f);

	initFlagDataFlag = false;
}

EnemyDebugScene::~EnemyDebugScene()
{
}

void EnemyDebugScene::Init()
{
	gameFlame = 2;
	oldEnemyType = -1;
	for (int i = 0; i < kidEnemyHandles.size(); ++i)
	{
		kidEnemyHandles[i] = 0;
	}

	player.data.color = { 0,255,0,255 };

	responePos = { -20.0f,0.0f,100.0f };

	eyePos = { 264.5f,145.0f,660.8f };
	targetPos = { 262.5f,145.0f,656.2f };
	angle = { 205.0f,165.0f };

	int lIndex = 0;
	enemies[lIndex] = std::make_unique<BattleshipEnemy>();
	misiles[lIndex][0] = std::make_unique<SplineMisileForBattleShip>();
	misiles[lIndex][1] = std::make_unique<SplineMisileForBattleShip>();
	misiles[lIndex][2] = std::make_unique<SplineMisileForBattleShip>();
	misiles[lIndex][3] = std::make_unique<SplineMisileForBattleShip>();
	misiles[lIndex][4] = std::make_unique<SplineMisileForBattleShip>();
	misiles[lIndex][5] = std::make_unique<SplineMisileForBattleShip>();
	misiles[lIndex][6] = std::make_unique<SplineMisileForBattleShip>();
	misiles[lIndex][7] = std::make_unique<SplineMisileForBattleShip>();
	++lIndex;
	enemies[lIndex] = std::make_unique<NormalEnemy>();
	++lIndex;
	enemies[lIndex] = std::make_unique<NormalMisileEnemy>();
	misiles[lIndex][0] = std::make_unique<SplineMisile>();
	++lIndex;
	enemies[lIndex] = std::make_unique<PopEnemy>();
	++lIndex;
	enemies[lIndex] = std::make_unique<BikeEnemy>();
	misiles[lIndex][0] = std::make_unique<SplineMisileForBikeEnemy>();
	misiles[lIndex][1] = std::make_unique<SplineMisileForBikeEnemy>();
	++lIndex;
	enemies[lIndex] = std::make_unique<SummonEnemy>();
	for (int i = 0; i < 20; ++i)
	{
		misiles[lIndex][i] = std::make_unique<PopEnemy>();
	}
}

void EnemyDebugScene::Finalize()
{
}

void EnemyDebugScene::Input()
{
	KeyBoradInputManager *input = KeyBoradInputManager::Instance();

#pragma region camera
	debugCameraMove = { 0,0,0 };
	float debugSpeed = 1;
	if (input->InputState(DIK_D))
	{
		debugCameraMove.x = -debugSpeed;
	}
	if (input->InputState(DIK_A))
	{
		debugCameraMove.x = debugSpeed;
	}
	if (input->InputState(DIK_W))
	{
		debugCameraMove.y = debugSpeed;
	}
	if (input->InputState(DIK_S))
	{
		debugCameraMove.y = -debugSpeed;
	}

	if (input->InputState(DIK_RIGHTARROW))
	{
		angle.x += debugSpeed;
	}
	if (input->InputState(DIK_LEFTARROW))
	{
		angle.x += -debugSpeed;
	}

	if (input->InputState(DIK_UPARROW))
	{
		angle.y += debugSpeed;
	}
	if (input->InputState(DIK_DOWNARROW))
	{
		angle.y += -debugSpeed;
	}

#pragma endregion

}

void EnemyDebugScene::Update()
{
	if (!initFlagDataFlag)
	{
		uvCheckModel.data.handle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::EnemyPath + "BattleShip/" + "BattleshipEnemy_HachOpen_anim.fbx");
		//FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::TestPath + "boneTest.fbx");	//モデル読み込み
		uvCheckModel.data.isPlayFlag = false;
		uvCheckModel.data.transform.pos = { 0.0f,0.0f,0.0f };
		uvCheckModel.data.transform.scale = { 100.0f,100.0f,100.0f };
		initFlagDataFlag = true;
	}
	gameFlame = 1 + specifiedEnemyType * 10;


	//デバック用
	eyePos = KazMath::CaluEyePosForDebug(eyePos, debugCameraMove, angle);
	targetPos = KazMath::CaluTargetPosForDebug(eyePos, angle.x);
	CameraMgr::Instance()->Camera(eyePos, targetPos, { 0.0f,1.0f,0.0f }, 0);


	ImGui::Begin("EnemyList");
	ImGui::InputInt("enemyIndex", &specifiedEnemyType);
	initFlag = ImGui::Button("Init");
	ImGui::SameLine();
	deadFlag = ImGui::Button("Dead");
	ImGui::SameLine();
	attackFlag = ImGui::Button("Attack");
	ImGui::DragFloat("ResponePosX", &responePos.x);
	ImGui::DragFloat("ResponePosY", &responePos.y);
	ImGui::DragFloat("ResponePosZ", &responePos.z);
	ImGui::End();

	//サイズ上限--------------------------------------
	if (specifiedEnemyType <= -1)
	{
		specifiedEnemyType = 0;
	}
	else if (enemies.size() <= specifiedEnemyType)
	{
		specifiedEnemyType = static_cast<int>(enemies.size() - 1);
	}
	//サイズ上限--------------------------------------


	//初期化--------------------------------------
	if (specifiedEnemyType != oldEnemyType || initFlag)
	{
		if (oldEnemyType <= -1)
		{
			oldEnemyType = 0;
		}

		//敵と子敵の死亡--------------------------------------
		if (oldEnemyType != -1)
		{
			enemies[oldEnemyType]->Dead();

			for (int i = 0; i < misiles[oldEnemyType].size(); ++i)
			{
				if (misiles[oldEnemyType][i] != nullptr)
				{
					misiles[oldEnemyType][i]->Dead();
				}
			}
		}
		kidEnemyHandles[oldEnemyType] = 0;
		//敵と子敵の死亡--------------------------------------

		//初期化--------------------------------------
		EnemyGenerateData lData;
		lData.initPos = responePos;
		lData.battleShipData.isShotFlag = true;
		lData.misileEnemy.isShotFlag = true;
		enemies[specifiedEnemyType]->Init(lData, true);
		//初期化--------------------------------------

		//簡易ツールの初期化--------------------------------------
		gameFlame = 0;
		initFlag = false;
		oldEnemyType = specifiedEnemyType;
		//簡易ツールの初期化--------------------------------------
	}
	//初期化--------------------------------------

	//死亡--------------------------------------
	if (deadFlag)
	{
		enemies[specifiedEnemyType]->Dead();
		deadFlag = false;
	}
	//死亡--------------------------------------


	//攻撃--------------------------------------
	if (attackFlag)
	{
		enemies[specifiedEnemyType]->DebugShot();
		kidEnemyHandles[specifiedEnemyType] = 0;
		attackFlag = false;
	}
	//攻撃--------------------------------------

	EnemyData *lEnemyData = enemies[specifiedEnemyType]->GetData().get();
	if (enemies[specifiedEnemyType]->GetData()->genarateData.enemyType != -1)
	{
		//生成する敵の種類
		misiles[specifiedEnemyType][kidEnemyHandles[specifiedEnemyType]]->Init(lEnemyData->genarateData.enemyGenerateData);

		++kidEnemyHandles[specifiedEnemyType];
		//追加したら終了処理を入れる
		lEnemyData->genarateData.Finalize();
	}

	for (int i = 0; i < misiles[specifiedEnemyType].size(); ++i)
	{
		if (misiles[specifiedEnemyType][i] != nullptr && misiles[specifiedEnemyType][i]->GetData()->timer <= 0)
		{
			misiles[specifiedEnemyType][i]->Hit();
		}
	}


	enemies[specifiedEnemyType]->Update();

	hitBox.data.transform.pos = *enemies[specifiedEnemyType]->GetData()->hitBox.center;
	float lScale = enemies[specifiedEnemyType]->GetData()->hitBox.radius;
	hitBox.data.transform.scale = { lScale ,lScale,lScale };
	hitBox.data.pipelineName = PIPELINE_NAME_COLOR_WIREFLAME;

	for (int i = 0; i < misiles[specifiedEnemyType].size(); ++i)
	{
		if (misiles[specifiedEnemyType][i] != nullptr && misiles[specifiedEnemyType][i]->GetData()->oprationObjData->initFlag)
		{
			misiles[specifiedEnemyType][i]->Update();
		}
	}

}

void EnemyDebugScene::Draw()
{
	RenderTargetStatus::Instance()->SetDoubleBufferFlame();
	RenderTargetStatus::Instance()->ClearDoubuleBuffer(BG_COLOR);



	bg.Draw();

	player.Draw();

	for (int i = 0; i < misiles[specifiedEnemyType].size(); ++i)
	{
		if (misiles[specifiedEnemyType][i] != nullptr)
		{
			misiles[specifiedEnemyType][i]->Draw();
		}
	}
	enemies[specifiedEnemyType]->Draw();
	//hitBox.Draw();
	//kidFbxModel.Draw();

	//battleShipObjModel.Draw();
	//boxModel.Draw();

	//FbxSkin *lData = FbxModelResourceMgr::Instance()->GetResourceData(uvCheckModel.data.handle.handle)->bone[0].fbxSkin;
	//FbxMesh *lData2 = FbxModelResourceMgr::Instance()->GetResourceData(uvCheckModel.data.handle.handle)->mesh;

	//lData = nullptr;
	//lData2 = nullptr;
	//uvCheckModel.Draw();
	//battleShipFbxModel->Draw();


}

int EnemyDebugScene::SceneChange()
{
	if (ControllerInputManager::Instance()->InputTrigger(XINPUT_GAMEPAD_START))
	{
		return -2;
	}
	else if (ControllerInputManager::Instance()->InputTrigger(XINPUT_GAMEPAD_BACK))
	{
		return 1;
	}
	else
	{
		return SCENE_NONE;
	}
}
