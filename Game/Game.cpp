#include "Game.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../KazLibrary/Input/ControllerInputManager.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../KazLibrary/Loader/ObjResourceMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../Game/Debug/ParameterMgr.h"
#include"../KazLibrary/Math/KazMath.h"
#include"../KazLibrary/Easing/easing.h"
#include<cmath>
#include<iostream>
#include"../KazLibrary/Sound/SoundManager.h"
#include"../Game/Effect/InstanceMeshParticle.h"
#include"../KazLibrary/Render/GPUParticleRender.h"

Game::Game(
	const std::array<std::array<ResponeData, KazEnemyHelper::ENEMY_NUM_MAX>, KazEnemyHelper::LAYER_LEVEL_MAX> &RESPONE_DATA,
	const std::array<std::shared_ptr<IStage>, KazEnemyHelper::STAGE_NUM_MAX> &STAGE_ARRAY,
	const std::array<KazMath::Color, KazEnemyHelper::STAGE_NUM_MAX> &BACKGROUND_COLOR,
	const std::array<std::array<KazEnemyHelper::ForceCameraData, 10>, KazEnemyHelper::STAGE_NUM_MAX> &CAMERA_ARRAY
) :LOG_FONT_SIZE(1.0f)
{

	for (int i = 0; i < smokeR.size(); ++i)
	{
		smokeR[i].data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::RelativeResourcePath + "Effect/Smoke/smoke9.png");
		smokeR[i].data.pipelineName = PIPELINE_NAME_SPRITE_Z_OFF;

		smokeR[i].data.transform.pos = { WIN_X / 2.0f,WIN_Y / 2.0f };
	}

	model = std::make_unique<ObjModelRender>();
	model->data.pipelineName = PIPELINE_NAME_COLOR_WIREFLAME;
	model->data.transform.pos = { 0.0f,0.0f,20.0f };
	model->data.transform.scale = { 5.0f,5.0f,5.0f };
	model->data.handle = ObjResourceMgr::Instance()->LoadModel(KazFilePathName::TestPath + "hamster.obj");
	model->data.colorData = { 255,0,0,255 };


	lineStartPoly[0].data.transform.pos = { 0.5f,0.15f,0.0f };
	lineStartPoly[1].data.transform.pos = { -0.5f,0.0f,-0.5f };
	lineStartPoly[2].data.transform.pos = { 0.5f,0.6f,0.5f };
	lineStartPoly[3].data.transform.pos = { 0.5f,1.0f,0.0f };
	lineStartPoly[4].data.transform.pos = { 0.5f,-1.0f,-1.0f };
	lineStartPoly[5].data.transform.pos = { -0.5f,-1.0f,0.0f };
	lineStartPoly[6].data.transform.pos = { 0.5f,0.3f,-0.3f };
	lineStartPoly[7].data.transform.pos = { 0.0f,-0.3f,-1.0f };


	blackTex.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TestPath + "Black.png");
	blackTex.data.pipelineName = PIPELINE_NAME_SPRITE_Z_ALWAYS;
	blackTex.data.transform.pos = { WIN_X / 2.0f,WIN_Y / 2.0f };
	blackTex.data.transform.scale = { 2000.0f,2000.0f };
	blackTex.data.colorData.color.a = 0;


	pressAButtonTex.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TestPath + "PressAButton.png");
	pressAButtonTex.data.transform.pos = { WIN_X / 2.0f,WIN_Y / 2.0f + 100.0f };
	pressAButtonTex.data.transform.scale = { 0.4f,0.4f };


	gameClearResourceHandle = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::UIPath + "GameClear.png");
	gameOverResouceHandle = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::GameOverPath + "GameOver.png");

	gameOverTex.data.handleData = gameClearResourceHandle;
	gameOverTex.data.transform.pos = { WIN_X / 2.0f,WIN_Y / 2.0f };
	gameOverTex.data.transform.scale = { 1.2f,1.2f };
	gameOverTex.data.pipelineName = PIPELINE_NAME_SPRITE_CUTALPHA;

	bgmSoundHandle = SoundManager::Instance()->LoadSoundMem(KazFilePathName::SoundPath + "Bgm.wav");
	lockSoundHandle = SoundManager::Instance()->LoadSoundMem(KazFilePathName::SoundPath + "Lock.wav", false);
	doneSoundHandle = SoundManager::Instance()->LoadSoundMem(KazFilePathName::SoundPath + "Done.wav");
	damageSoundHandle = SoundManager::Instance()->LoadSoundMem(KazFilePathName::SoundPath + "EenemyDamage.wav");

	SoundManager::Instance()->StopSoundMem(bgmSoundHandle);
	SoundManager::Instance()->PlaySoundMem(bgmSoundHandle, 1, true);

	CameraMgr::Instance()->CameraSetting(60.0f, 100000.0f, 0);
	CameraMgr::Instance()->CameraSetting(60.0f, 100000.0f, 1);


	stringLog.Init({ 50.0f,50.0f });
	mainRenderTarget.data.transform.pos = { WIN_X / 2.0f,WIN_Y / 2.0f };
	mainRenderTarget.data.pipelineName = PIPELINE_NAME_SPRITE_NOBLEND;

	playerModel.data.handle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::PlayerPath + "CH_Right_Back_Anim.fbx");
	headModel.data.handle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::PlayerPath + "CH_Model_Head.fbx");

	logoutWindow = std::make_unique<StringWindow>();



	responeData = RESPONE_DATA;
	cameraMoveArray = CAMERA_ARRAY;
	stages = STAGE_ARRAY;

	//操作可能OBJを纏めて生成する処理----------------------------------------------------------------
	KazEnemyHelper::GenerateEnemy(enemies, responeData, enemiesHandle, enemyHitBoxArray);
	//敵を纏めて生成する処理----------------------------------------------------------------

	for (int i = 0; i < BACKGROUND_COLOR.size(); ++i)
	{
		renderTarget[i] = std::make_unique<GameRenderTarget>(BACKGROUND_COLOR[i]);

		if (i + 1 < BACKGROUND_COLOR.size())
		{
			nextRenderTarget[i + 1] = std::make_unique<GameRenderTarget>(BACKGROUND_COLOR[i + 1]);
		}
	}

	std::vector<InitMeshCollisionData> lInitCollisionData = stages[0]->collisionArrrayData;


	for (int i = 0; i < stages[0]->blockHitBox.size(); ++i)
	{
		enemyHitBoxArray.emplace_back(stages[0]->blockHitBox[i]);
	}
	p1 = { 0.0f,0.0f,0.0f };
	t1.center = &p1;
	//enemyHitBoxArray.emplace_back(&t1);

	meshCollision = std::make_unique<InstanceMeshCollision>(lInitCollisionData, enemyHitBoxArray);


	particleRender = std::make_unique<GPUParticleRender>();
	meshParticle = std::make_unique<InstanceMeshParticle>(particleRender.get());

	for (int enemyType = 0; enemyType < responeData.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < responeData[enemyType].size(); ++enemyCount)
		{
			if (enemies[enemyType][enemyCount] == nullptr)
			{
				continue;
			}
			if (!enemies[enemyType][enemyCount]->GetData()->meshParticleFlag)
			{
				continue;
			}
			meshParticle->AddMeshData(enemies[enemyType][enemyCount]->GetData()->meshParticleData[0]->meshParticleData);
		}
	}

	for (int i = 0; i < stages[0]->particleArrrayData.size(); ++i)
	{
		meshParticle->AddMeshData(stages[0]->particleArrrayData[i]);
	}
}

Game::~Game()
{
	//レンダーターゲットの破棄
	SoundManager::Instance()->StopSoundMem(bgmSoundHandle);
}

void Game::Init(bool SKIP_FLAG)
{
	player.Init(KazMath::Transform3D().pos, true, false);
	cursor.Init();


	stageUI.Init();

	sceneNum = SCENE_NONE;

	const int NO_ENEMY_NUM = 0;
	BufferMemorySize enemySize;
	enemySize.startSize = NO_ENEMY_NUM;
	enemySize.endSize = KazEnemyHelper::ENEMY_NUM_MAX;

	BufferMemorySize eventSize;
	enemySize.startSize = KazEnemyHelper::ENEMY_NUM_MAX;
	enemySize.endSize = KazEnemyHelper::ENEMY_NUM_MAX + 50;






	//ゲームループの初期化----------------------------------------------------------------
	gameStartFlag = false;
	cameraModeChangeFlag = false;
	gameFlame = 0;
	changeLayerLevelMaxTime[0] = KazMath::ConvertSecondToFlame(70);
	changeLayerLevelMaxTime[1] = KazMath::ConvertSecondToFlame(70);
	changeLayerLevelMaxTime[2] = KazMath::ConvertSecondToFlame(71);
	//ゴールに触れ無かった場合に次のステージに移動する際の最大フレーム数
	for (int i = 3; i < changeLayerLevelMaxTime.size(); ++i)
	{
		changeLayerLevelMaxTime[i] = KazMath::ConvertSecondToFlame(70);
	}
	gameStageLevel = 0;
	stageNum = gameStageLevel;
	//ゲームループの初期化----------------------------------------------------------------


	testEnemyPos = { 0.0f,0.0f,100.0f };

	appearGoalBoxPos[0] = { 0.0f,10.0f,40.0f };
	appearGoalBoxPos[1] = { 0.0f,-15.0f,40.0f };
	appearGoalBoxPos[2] = { 20.0f,-5.0f,40.0f };
	responeGoalBoxPos = { -10.0f,-100.0f,40.0f };
	goalBox.Init(responeGoalBoxPos);
	initAppearFlag = false;


	gameOverFlag = false;
	readyToBlackOutFlag = false;
	readyToBlackOutToGoTitleFlag = false;
	flashFlag = false;
	flashTimer = 0;


	tutorialWindow.Init();

	rocketIndex = 0;
	fireIndex = 0;
	cameraWork.Init();

	tutorial.Init(SKIP_FLAG);
	portalEffect.Init();

	isGameOverFlag = false;
	isGameClearFlag = false;

	gameClearFlag = false;
	sceneChangeFlag = false;


	d = false;
	prepareToClearFlag = false;
	initEndLogFlag = false;
	gameClearTimer = 0;




	meshCollision->Init(particleRender.get());
	meshParticle->Init();

}

void Game::Finalize()
{
	tutorial.Finalize();
}

void Game::Input()
{


	KeyBoradInputManager *input = KeyBoradInputManager::Instance();
	ControllerInputManager *inputController = ControllerInputManager::Instance();


	if (tutorial.tutorialFlag)
	{
		tutorial.Input();

		if (inputController->InputTrigger(XINPUT_GAMEPAD_START))
		{
			sceneNum = -3;
		}
	}


	bool upFlag = false;
	bool downFlag = false;
	bool leftFlag = false;
	bool rightFlag = false;
	bool doneFlag = false;
	bool releaseFlag = false;

	const int DEAD_ZONE = 3000;
	if (inputController->InputState(XINPUT_GAMEPAD_A))
	{
		doneFlag = true;
	}
	if (inputController->InputRelease(XINPUT_GAMEPAD_A))
	{
		releaseFlag = true;
	}
	if (inputController->InputStickState(LEFT_STICK, UP_SIDE, DEAD_ZONE))
	{
		upFlag = true;
	}
	if (inputController->InputStickState(LEFT_STICK, DOWN_SIDE, DEAD_ZONE))
	{
		downFlag = true;
	}
	if (inputController->InputStickState(LEFT_STICK, LEFT_SIDE, DEAD_ZONE))
	{
		leftFlag = true;
	}
	if (inputController->InputStickState(LEFT_STICK, RIGHT_SIDE, DEAD_ZONE))
	{
		rightFlag = true;
	}

	//ゲームオーバー画面でタイトル画面に戻る処理
	if (inputController->InputTrigger(XINPUT_GAMEPAD_A) && gameOverFlag)
	{
		readyToBlackOutToGoTitleFlag = true;
	}
	//ゲームクリアでタイトルに戻る
	if (inputController->InputTrigger(XINPUT_GAMEPAD_A) && gameClearFlag)
	{
		readyToBlackOutToGoTitleFlag = true;
	}


	if (input->InputTrigger(DIK_O))
	{
		sceneNum = -3;
	}


	KazMath::Vec2<float> joyStick;
	joyStick.x = inputController->GetJoyStickLXNum(0) / 32767.0f;
	joyStick.y = inputController->GetJoyStickLYNum(0) / 32767.0f;

	cursor.Input
	(
		upFlag,
		downFlag,
		leftFlag,
		rightFlag,
		doneFlag,
		releaseFlag,
		joyStick
	);

	if (input->InputTrigger(DIK_SPACE))
	{
		goalBox.Init(responeGoalBoxPos);
		goalBox.Appear(appearGoalBoxPos[stageNum]);
	}



}

void Game::Update()
{
	tutorial.handle = renderTarget[0]->GetGameRenderTargetHandle();
	if (tutorial.tutorialFlag)
	{
		tutorial.Update();
		stages[0]->Update();
		cursor.Update();
		cameraWork.Update(cursor.GetValue(), &player.pos, cameraModeChangeFlag);
		player.Update();
		return;
	}
	stages[0]->startFlag = true;

	for (int i = 0; i < lineStartPoly.size(); ++i)
	{
		lineStartPoly[i].data.pipelineName = PIPELINE_NAME_COLOR_MULTITEX;
		lineStartPoly[i].data.transform.scale = { 0.1f,0.1f,0.1f };
		lineStartPoly[i].data.color = { 255,0,0,255 };
		lineEffectArrayData[i].startPos = lineStartPoly[i].data.transform.pos + player.pos;
	}
	{
		int lineArrayNum = static_cast<int>(lineStartPoly.size());
		for (int i = lineArrayNum; i < lineArrayNum * 2; ++i)
		{
			lineEffectArrayData[i].startPos = lineStartPoly[i - lineArrayNum].data.transform.pos + player.pos;
		}
	}


	for (int i = 0; i < cameraMoveArray[stageNum].size(); ++i)
	{
		if (cameraMoveArray[stageNum][i].flame <= gameFlame && cameraMoveArray[stageNum][i].flame != -1)
		{
			cameraWork.ForceCamera(cameraMoveArray[stageNum][i].dir);
			player.ForceAnim(static_cast<int>(cameraMoveArray[stageNum][i].dir));
		}
	}

	cameraWork.Update(cursor.GetValue(), &player.pos, cameraModeChangeFlag);
	eyePos = cameraWork.GetEyePos();
	targetPos = cameraWork.GetTargetPos();


	const float MAX_ANGLE = 120.0f;
	const float DEFAULT_ANGLE = 60.0f;
	if (!portalEffect.IsFinish())
	{
		if (portalEffect.DrawNextPortal())
		{
			cameraAngle = MAX_ANGLE + portalEffect.GetNextRate() * -(MAX_ANGLE - DEFAULT_ANGLE);
		}
		else
		{
			cameraAngle = DEFAULT_ANGLE + portalEffect.GetRate() * (MAX_ANGLE - DEFAULT_ANGLE);
		}
	}
	else
	{
		cameraAngle = DEFAULT_ANGLE;
	}

	if (gameClearFlag)
	{
		cameraAngle = 60.0f;
	}
	CameraMgr::Instance()->CameraSetting(cameraAngle, 100000.0f, 0);
	CameraMgr::Instance()->Camera(eyePos, targetPos, { 0.0f,1.0f,0.0f }, 0);

	//敵が一通り生成終わった際に登場させる----------------------------------------------------------------
	if (changeLayerLevelMaxTime[gameStageLevel] <= gameFlame && !initAppearFlag)
		//if (100 <= gameFlame && !initAppearFlag)
	{
		goalBox.Appear(appearGoalBoxPos[stageNum]);
		initAppearFlag = true;
	}
	//敵が一通り生成終わった際に登場させる----------------------------------------------------------------

	changeStageFlag = false;

	portalEffect.Update(player.pos);
	//全部隠れたら次ステージの描画をする
	if (portalEffect.IsFinish())
	{
		//ゲームループの初期化----------------------------------------------
		++gameStageLevel;
		gameFlame = 0;
		//ゲームループの初期化----------------------------------------------

		//ステージの切り替え----------------------------------------------
		++stageNum;
		changeStageFlag = true;
		//ステージの切り替え----------------------------------------------

		//ゴールボックスの初期化----------------------------------------------
		initAppearFlag = false;
		goalBox.Init(responeGoalBoxPos);
		//ゴールボックスの初期化----------------------------------------------

		stages[stageNum]->startFlag = true;

		rocketIndex = 0;
		fireIndex = 0;

		stageUI.Init();
		stageUI.AnnounceStage(stageNum + 1);

		portalEffect.Finalize();
	}


	//ポータル演出開始
	if (goalBox.startPortalEffectFlag && !portalEffect.IsStart())
	{
		if (stages.size() <= stageNum + 1)
		{
			portalEffect.Init(false);
		}
		else
		{
			portalEffect.Init();
		}
		portalEffect.Start();
	}

	//入った後の初期化
	if (portalEffect.disappearFlag && goalBox.startPortalEffectFlag)
	{
		if (stages.size() <= stageNum + 1)
		{
			prepareToClearFlag = true;
		}
		goalBox.startPortalEffectFlag = false;
	}

	if (prepareToClearFlag)
	{
		++gameClearTimer;
	}

	if (120 <= gameClearTimer && !initEndLogFlag)
	{
		logoutWindow->Init("log-out");
		initEndLogFlag = true;
	}

#pragma region 生成処理
	//敵を追加で初期化する処理----------------------------------------------------------------
	KazEnemyHelper::AddEnemy(enemies, responeData, addEnemiesHandle, gameFlame, gameStageLevel);
	//敵を追加で初期化する処理----------------------------------------------------------------


	//敵をどのタイミングで初期化する処理----------------------------------------------------------------
	for (int enemyType = 0; enemyType < responeData.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < responeData[enemyType].size(); ++enemyCount)
		{
			bool enableToUseThisDataFlag = responeData[enemyType][enemyCount].layerLevel != -1;
			bool readyToStartFlag = responeData[enemyType][enemyCount].flame <= gameFlame &&
				responeData[enemyType][enemyCount].layerLevel == gameStageLevel;

			if (enableToUseThisDataFlag && readyToStartFlag && enemies[enemyType][enemyCount] != nullptr && !enemies[enemyType][enemyCount]->GetData()->oprationObjData->initFlag)
			{
#ifdef _DEBUG
				const float L_SCALE = enemies[enemyType][enemyCount]->GetData()->hitBox.radius;
				enemyHitBox[enemyType][enemyCount].data.transform.scale = { L_SCALE ,L_SCALE ,L_SCALE };
				enemyHitBox[enemyType][enemyCount].data.pipelineName = PIPELINE_NAME_COLOR_WIREFLAME;
#endif

				enemies[enemyType][enemyCount]->OnInit(responeData[enemyType][enemyCount].generateData.useMeshPaticleFlag);
				enemies[enemyType][enemyCount]->Init(responeData[enemyType][enemyCount].generateData, false);

				//メッシュパーティクルの付与
				for (int i = 0; i < enemies[enemyType][enemyCount]->GetData()->meshParticleData.size(); ++i)
				{
					if (enemyType == ENEMY_TYPE_BATTLESHIP_MISILE ||
						enemyType == ENEMY_TYPE_BIKE_MISILE ||
						enemyType == ENEMY_TYPE_MISILE_SPLINE
						)
					{
						continue;
					}

					RESOURCE_HANDLE lHandle = enemies[enemyType][enemyCount]->GetData()->meshParticleData[i]->resourceHandle;
					std::vector<DirectX::XMFLOAT4>lVertData = FbxModelResourceMgr::Instance()->GetResourceData(lHandle)->vertFloat4Data;

					float lScale = 0.5f;
					switch (enemyType)
					{
					case ENEMY_TYPE_BATTLESHIP:
						lScale = 1.0f;
						break;
					default:
						break;
					}

					//deadParticle->AddData(
					//	enemies[enemyType][enemyCount]->GetData()->deadParticleData
					//);

					/*deadParticleArray[enemyType][enemyCount].push_back(std::make_unique<DeadParticle>(
						enemies[enemyType][enemyCount]->GetData()->meshParticleData[0]->meshParticleData.vertData,
						enemies[enemyType][enemyCount]->GetData()->meshParticleData[0]->meshParticleData.triagnleData.x,
						deadParticleRender.get(),
						lScale)
					);*/
				}
				if (enemies[enemyType][enemyCount]->GetData()->meshParticleFlag)
				{
					continue;
				}

				switch (enemyType)
				{
				case ENEMY_TYPE_NORMAL:
					lightEffect[rocketIndex].Init(enemies[enemyType][enemyCount]->GetData()->hitBox.center, KazMath::Vec3<float>(0.0f, 0.0f, 20.0f), true, &enemies[enemyType][enemyCount]->GetData()->oprationObjData->enableToHitFlag, &enemies[enemyType][enemyCount]->GetData()->radius, &enemies[enemyType][enemyCount]->GetData()->startFlag);
					fireEffect[fireIndex].Init(enemies[enemyType][enemyCount]->GetData()->hitBox.center, KazMath::Vec3<float>(0.0f, 0.0f, 45.0f), true, &enemies[enemyType][enemyCount]->GetData()->oprationObjData->enableToHitFlag);
					++rocketIndex;
					++fireIndex;
					break;

				case ENEMY_TYPE_MISILE:
					lightEffect[rocketIndex].Init(enemies[enemyType][enemyCount]->GetData()->hitBox.center, KazMath::Vec3<float>(0.0f, 0.0f, 10.0f), true, &enemies[enemyType][enemyCount]->GetData()->oprationObjData->enableToHitFlag, &enemies[enemyType][enemyCount]->GetData()->radius, &enemies[enemyType][enemyCount]->GetData()->startFlag);
					fireEffect[rocketIndex].Init(enemies[enemyType][enemyCount]->GetData()->hitBox.center, KazMath::Vec3<float>(0.0f, 0.0f, 35.0f), true, &enemies[enemyType][enemyCount]->GetData()->oprationObjData->enableToHitFlag);
					++rocketIndex;
					++fireIndex;
					break;

				case ENEMY_TYPE_BATTLESHIP_MISILE:
					lightEffect[rocketIndex].Init(enemies[enemyType][enemyCount]->GetData()->hitBox.center, KazMath::Vec3<float>(0.0f, 0.0f, 0.0f), false, &enemies[enemyType][enemyCount]->GetData()->oprationObjData->enableToHitFlag, &enemies[enemyType][enemyCount]->GetData()->radius, &enemies[enemyType][enemyCount]->GetData()->startFlag);
					++rocketIndex;
					break;

				case ENEMY_TYPE_BIKE_MISILE:
					lightEffect[rocketIndex].Init(enemies[enemyType][enemyCount]->GetData()->hitBox.center, KazMath::Vec3<float>(0.0f, 0.0f, 0.0f), false, &enemies[enemyType][enemyCount]->GetData()->oprationObjData->enableToHitFlag, &enemies[enemyType][enemyCount]->GetData()->radius, &enemies[enemyType][enemyCount]->GetData()->startFlag);
					++rocketIndex;
					break;

				case ENEMY_TYPE_MISILE_SPLINE:
					lightEffect[rocketIndex].Init(enemies[enemyType][enemyCount]->GetData()->hitBox.center, KazMath::Vec3<float>(0.0f, 0.0f, 0.0f), false, &enemies[enemyType][enemyCount]->GetData()->oprationObjData->enableToHitFlag, &enemies[enemyType][enemyCount]->GetData()->radius, &enemies[enemyType][enemyCount]->GetData()->startFlag);
					++rocketIndex;
					break;
				default:
					break;
				}
			}
		}
	}
	//敵をどのタイミングで初期化する処理----------------------------------------------------------------
#pragma endregion


#pragma region 敵ロックオン
	for (int enemyType = 0; enemyType < enemies.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < enemies[enemyType].size(); ++enemyCount)
		{
			//生成されている、初期化している敵のみ更新処理を通す
			bool enableToUseDataFlag = enemies[enemyType][enemyCount] != nullptr && enemies[enemyType][enemyCount]->GetData()->oprationObjData->initFlag;
			if (enableToUseDataFlag)
			{
				//敵のデータのポインタを代入
				EnemyData *enemyData = enemies[enemyType][enemyCount]->GetData().get();

				//ロックオン判定
				bool enableToLockOnNumFlag = cursor.LockOn();
				bool enableToLockOnEnemyFlag = enemies[enemyType][enemyCount]->IsAlive() && !enemies[enemyType][enemyCount]->LockedOrNot();
				bool hitFlag = CollisionManager::Instance()->CheckRayAndSphere(cursor.hitBox, enemyData->hitBox);
				if (hitFlag &&
					enableToLockOnNumFlag &&
					enableToLockOnEnemyFlag &&
					!cursor.releaseFlag)
				{
					SoundManager::Instance()->PlaySoundMem(lockSoundHandle, 1);

					//カーソルのカウント数を増やす
					cursor.Hit(enemyData->hitBox.center);
					//敵が当たった情報を書く
					enemies[enemyType][enemyCount]->Hit();

					stringLog.WriteLog(enemies[enemyType][enemyCount]->GetData()->oprationObjData->name, LOG_FONT_SIZE);

					//線演出をかける際にどの配列を使用するか決める
					for (int i = 0; i < lineEffectArrayData.size(); ++i)
					{
						if (!lineEffectArrayData[i].usedFlag)
						{
							lineLevel[i].Attack(lineEffectArrayData[i].startPos, *enemyData->hitBox.center);
							lineEffectArrayData[i].usedFlag = true;
							lineEffectArrayData[i].lineIndex = i;
							lineEffectArrayData[i].enemyTypeIndex = enemyType;
							lineEffectArrayData[i].enemyIndex = enemyCount;
							break;
						}
					}
				}
			}
		}
	}


#pragma region イベントロックオン
	{
		//ロックオン判定
		bool enableToLockOnNumFlag = cursor.LockOn();
		bool enableToLockOnEnemyFlag = goalBox.IsAlive() && !goalBox.LockedOrNot();
		bool hitFlag = CollisionManager::Instance()->CheckRayAndSphere(cursor.hitBox, goalBox.hitBox);
		if (hitFlag &&
			enableToLockOnNumFlag &&
			enableToLockOnEnemyFlag)
		{
			SoundManager::Instance()->PlaySoundMem(lockSoundHandle, 1);

			//カーソルのカウント数を増やす
			cursor.Hit(goalBox.hitBox.center);
			goalBox.Hit();

			//線演出をかける際にどの配列を使用するか決める
			for (int i = 0; i < lineEffectArrayData.size(); ++i)
			{
				if (!lineEffectArrayData[i].usedFlag)
				{
					lineLevel[i].Attack(lineEffectArrayData[i].startPos, *goalBox.hitBox.center);
					lineEffectArrayData[i].usedFlag = true;
					lineEffectArrayData[i].lineIndex = i;
					lineEffectArrayData[i].eventType = 0;
					break;
				}
			}
		}
	}

#pragma endregion


	for (int enemyType = 0; enemyType < enemies.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < enemies[enemyType].size(); ++enemyCount)
		{
			//生成されている、初期化している敵のみ更新処理を通す
			bool enableToUseDataFlag = enemies[enemyType][enemyCount] != nullptr && enemies[enemyType][enemyCount]->GetData()->oprationObjData->initFlag;
			if (enableToUseDataFlag)
			{
				//敵のデータのポインタを代入
				EnemyData *enemyData = enemies[enemyType][enemyCount]->GetData().get();

				for (int i = 0; i < lineEffectArrayData.size(); ++i)
				{
					bool sameEnemyFlag = lineEffectArrayData[i].enemyTypeIndex == enemyType && lineEffectArrayData[i].enemyIndex == enemyCount;
					if (lineEffectArrayData[i].usedFlag && sameEnemyFlag)
					{
						enemyData->oprationObjData->lockOnFlag = true;
						break;
					}
					else
					{
						enemyData->oprationObjData->lockOnFlag = false;
					}
				}
			}
		}
	}

	for (int i = 0; i < lineEffectArrayData.size(); ++i)
	{
		bool sameEventFlag = lineEffectArrayData[i].eventType == 0 && lineEffectArrayData[i].lineIndex == i;
		if (lineEffectArrayData[i].usedFlag && sameEventFlag)
		{
			goalBox.iOperationData.lockOnFlag = true;
			break;
		}
		else
		{
			goalBox.iOperationData.lockOnFlag = false;
		}
	}





	for (int enemyType = 0; enemyType < enemies.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < enemies[enemyType].size(); ++enemyCount)
		{
			//生成されている、初期化している敵のみ更新処理を通す
			bool enableToUseDataFlag = enemies[enemyType][enemyCount] != nullptr && enemies[enemyType][enemyCount]->GetData()->oprationObjData->initFlag;
			if (enableToUseDataFlag)
			{
				//敵のデータのポインタを代入
				EnemyData *enemyData = enemies[enemyType][enemyCount]->GetData().get();

				for (int i = 0; i < lineEffectArrayData.size(); ++i)
				{
					bool sameEnemyFlag = lineEffectArrayData[i].enemyTypeIndex == enemyType && lineEffectArrayData[i].enemyIndex == enemyCount;
					bool releaseFlag = enemyData->outOfStageFlag || enemyData->timer <= 0;

					if (lineEffectArrayData[i].usedFlag && sameEnemyFlag && releaseFlag)
					{
						int lineIndex = lineEffectArrayData[i].lineIndex;
						lineLevel[lineIndex].Release();
						cursor.SubCount(1);
						break;
					}
				}
			}
		}
	}



#pragma region 線のロックオン解放

	//線がたどり着いたら敵を死亡させる
	for (int i = 0; i < lineEffectArrayData.size(); ++i)
	{
		if (lineEffectArrayData[i].usedFlag)
		{
			int lineIndex = lineEffectArrayData[i].lineIndex;
			int enemyTypeIndex = lineEffectArrayData[i].enemyTypeIndex;
			int enemyIndex = lineEffectArrayData[i].enemyIndex;
			int eventIndex = lineEffectArrayData[i].eventType;

			if (eventIndex != -1)
			{
				break;
			}

			//演出を合わせてダメージと死亡をやる
			if (lineLevel[lineIndex].lineReachObjFlag && !lineLevel[lineIndex].alreadyReachedFlag && enemies[enemyTypeIndex][enemyIndex]->IsAlive())
			{
				SoundManager::Instance()->PlaySoundMem(damageSoundHandle, 1);
				lineLevel[lineIndex].alreadyReachedFlag = true;
			}
			else if (lineLevel[lineIndex].lineReachObjFlag && !enemies[enemyTypeIndex][enemyIndex]->IsAlive() && !lineEffectArrayData[i].hitFlag)
			{
				enemies[enemyTypeIndex][enemyIndex]->Dead();
				lineEffectArrayData[i].hitFlag = true;

				//ヒット時の円演出
				for (int hitEffectIndex = 0; hitEffectIndex < hitEffect.size(); ++hitEffectIndex)
				{
					if (!hitEffect[hitEffectIndex].IsAlive())
					{
						hitEffect[hitEffectIndex].Start(*enemies[enemyTypeIndex][enemyIndex]->GetData()->hitBox.center);
						break;
					}
				}

				stages[stageNum]->hitFlag = true;
			}
		}
	}

	//線演出が終わった際にリンクのデータをリセットする
	for (int i = 0; i < lineEffectArrayData.size(); ++i)
	{
		if (lineEffectArrayData[i].usedFlag)
		{
			int lineIndex = lineEffectArrayData[i].lineIndex;
			if (lineLevel[lineIndex].allFinishFlag)
			{
				lineEffectArrayData[i].Reset();
			}
		}
	}

	if (cursor.Release() && cursor.GetCount() != 0)
	{
		const std::string lLog("Fire_[" + std::to_string(cursor.GetCount()) + "x]");
		stringLog.WriteLog(lLog, LOG_FONT_SIZE);
	}

#pragma endregion


#pragma endregion


	//攻撃----------------------------------------------
	for (int enemyType = 0; enemyType < enemies.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < enemies[enemyType].size(); ++enemyCount)
		{
			//生成されている、初期化している敵のみ更新処理を通す
			bool enableToUseDataFlag = enemies[enemyType][enemyCount] != nullptr && enemies[enemyType][enemyCount]->GetData()->oprationObjData->enableToHitFlag;
			if (enableToUseDataFlag)
			{
				//時間が0秒以下ならプレイヤーに当たったと判断する
				if (enemies[enemyType][enemyCount]->GetData()->timer <= 0)
				{
					player.Hit();
					//enemies[enemyType][enemyCount]->Dead();
				}
			}
		}
	}
	//攻撃----------------------------------------------


	//ゲームオーバーかゲームクリアの表示----------------------------------------------
	if (!player.IsAlive() && !gameOverFlag)
	{
		isGameOverFlag = true;
		readyToBlackOutFlag = true;
	}

	if (logoutWindow->IsFinish() && !d)
	{
		isGameClearFlag = true;
		readyToBlackOutFlag = true;
		d = true;
	}



	if (readyToBlackOutFlag || readyToBlackOutToGoTitleFlag)
	{
		//暗転
		if (blackTex.data.colorData.color.a <= 255.0f)
		{
			blackTex.data.colorData.color.a += 5;
		}
		//タイトル画面に戻る
		else if (readyToBlackOutToGoTitleFlag && 255 <= blackTex.data.colorData.color.a && !sceneChangeFlag)
		{
			sceneNum = 0;
			sceneChangeFlag = true;
		}
		//ゲームオーバー画面を表示
		else if (isGameOverFlag)
		{
			gameOverFlag = true;
			readyToBlackOutFlag = false;
		}
		//ゲームクリア画面表示
		else if (isGameClearFlag)
		{
			gameClearFlag = true;
			readyToBlackOutFlag = false;
		}
	}
	else
	{
		//明転
		if (0 <= blackTex.data.colorData.color.a)
		{
			blackTex.data.colorData.color.a -= 5;
		}
	}

	if (gameOverFlag || gameClearFlag)
	{
		++flashTimer;
		if (60 <= flashTimer)
		{
			flashFlag = !flashFlag;
			flashTimer = 0;
		}
	}
	//ゲームオーバーかゲームクリアの表示----------------------------------------------


	if (!gameOverFlag && !gameClearFlag)
	{
#pragma region 更新処理
		goalBox.releaseFlag = cursor.releaseFlag;

		//更新処理----------------------------------------------------------------
		player.cameraRate = cursor.GetValue();

		player.Update();
		cursor.Update();
		goalBox.Update();
		stageUI.Update();
		stages[stageNum]->Update();
		stages[stageNum]->hitFlag = false;
		tutorialWindow.Update();
		logoutWindow->Update();
		if (stageNum == 0)
		{
			meshParticle->Compute();
			meshCollision->Compute();
		}

		for (int i = 0; i < hitEffect.size(); ++i)
		{
			hitEffect[i].Update();
		}

		//配列外を超えない処理
		if (stageNum + 1 < stages.size())
		{
			stages[stageNum + 1]->Update();
		}
		//ロックオンのリリース処理
		if (cursor.releaseFlag)
		{
			for (int i = 0; i < lineEffectArrayData.size(); ++i)
			{
				if (lineEffectArrayData[i].usedFlag)
				{
					int lineIndex = lineEffectArrayData[i].lineIndex;
					lineLevel[lineIndex].ReleaseShot();
				}
			}
		}

		//ロックオン中に必要なデータ
		for (int i = 0; i < lineEffectArrayData.size(); ++i)
		{
			if (lineEffectArrayData[i].usedFlag)
			{
				int lineIndex = lineEffectArrayData[i].lineIndex;
				//イベントブロックかどうか判断する
				if (lineEffectArrayData[i].eventType != -1)
				{
					KazMath::Vec3<float> pos = *goalBox.hitBox.center;
					lineLevel[lineIndex].CalucurateDistance(player.pos, pos);
				}
				else
				{
					int enemyTypeIndex = lineEffectArrayData[i].enemyTypeIndex;
					int enemyIndex = lineEffectArrayData[i].enemyIndex;
					KazMath::Vec3<float> pos = *enemies[enemyTypeIndex][enemyIndex]->GetData()->hitBox.center;
					lineLevel[lineIndex].CalucurateDistance(player.pos, pos);
				}
			}
		}


		for (int i = 0; i < lineLevel.size(); ++i)
		{
			if (lineEffectArrayData[i].usedFlag)
			{
				lineLevel[i].playerPos = lineEffectArrayData[i].startPos;
			}

			if (!lineLevel[i].initFlag)
			{
				lineEffectArrayData[i].Reset();
			}

			lineLevel[i].Update();
		}


		isEnemyNotMoveFlag = true;
		//敵の更新処理
		for (int enemyType = 0; enemyType < enemies.size(); ++enemyType)
		{
			for (int enemyCount = 0; enemyCount < enemies[enemyType].size(); ++enemyCount)
			{
				//生成されている、初期化している敵のみ更新処理を通す
				bool enableToUseDataFlag =
					enemies[enemyType][enemyCount] != nullptr &&
					enemies[enemyType][enemyCount]->GetData()->oprationObjData->initFlag &&
					!enemies[enemyType][enemyCount]->GetData()->outOfStageFlag;
				if (enableToUseDataFlag)
				{
					if (enemies[enemyType][enemyCount]->GetData()->oprationObjData->enableToHitFlag)
					{
						enemies[enemyType][enemyCount]->SetLight(cursor.hitBox.dir, enemies[enemyType][enemyCount]->GetData()->objFlag);
					}
					enemies[enemyType][enemyCount]->Update();
#ifdef _DEBUG
					enemyHitBox[enemyType][enemyCount].data.transform.pos = *enemies[enemyType][enemyCount]->GetData()->hitBox.center;
#endif
				}

				//一体でも敵が動いていたらそれを知らせるフラグを上げる
				if (enableToUseDataFlag && enemies[enemyType][enemyCount]->GetData()->oprationObjData->enableToHitFlag)
				{
					isEnemyNotMoveFlag = false;
				}
			}
		}
		//更新処理----------------------------------------------------------------


		for (int i = 0; i < fireEffect.size(); ++i)
		{
			fireEffect[i].Update();
		}
		for (int i = 0; i < lightEffect.size(); ++i)
		{
			lightEffect[i].Update();
		}

#pragma endregion
		stringLog.Update();
	}


	//ゲームループの経過時間----------------------------------------------------------------

	//ゲーム開始
	if (!tutorial.tutorialFlag && !gameStartFlag)
	{
		//ステージ番号知らせる
		stageUI.Init();
		stageUI.AnnounceStage(stageNum + 1);
		gameStartFlag = true;
	}


	if (gameStartFlag)
	{
		if (isEnemyNotMoveFlag)
		{
			++notMoveTimer;
		}
		else
		{
			notMoveTimer = 0;
		}

		//敵が何もしていない時間が一定時間を超えたらゲーム内時間を早める
		if (KazMath::ConvertSecondToFlame(CHANGE_GMAE_FLAME_SPEED_MAX_TIME) <= notMoveTimer)
		{
			gameSpeed = 60;
		}
		else
		{
			gameSpeed = 1;
		}

		gameFlame += gameSpeed;
	}

	if (60 * 4 <= gameFlame)
	{
		//チュートリアル用のUI非表示
		tutorialWindow.End();
	}

	//ゲームループの経過時間----------------------------------------------------------------

}

void Game::Draw()
{
	particleRender->InitCount();

	if (tutorial.tutorialFlag)
	{
		tutorial.Draw();
	}
	else
	{
		RenderTargetStatus::Instance()->SetDoubleBufferFlame();
		RenderTargetStatus::Instance()->ClearDoubuleBuffer(BG_COLOR);
	}
	//中間演出の描画
	portalEffect.Draw(&cursor);

	//次のポータルの描画
	if (portalEffect.DrawNextPortal())
	{
		int lStageNum = stageNum + 1;
		if (lStageNum < 0)
		{
			lStageNum = 1;
		}
		else if (stages.size() <= lStageNum)
		{
			lStageNum = static_cast<int>(stages.size() - 1);
		}

		nextRenderTarget[lStageNum]->SetRenderTarget();
		CameraMgr::Instance()->Camera(eyePos, targetPos, { 0.0f,1.0f,0.0f }, 1);
		player.Draw();
		stages[lStageNum]->SetCamera(1);
		stages[lStageNum]->Draw();
		nextRenderTarget[lStageNum]->Draw();


		portalEffect.nextPortalRender.data.handleData = nextRenderTarget[lStageNum]->GetGameRenderTargetHandle();

	}

	if (!gameOverFlag && !gameClearFlag)
	{
		renderTarget[stageNum]->SetRenderTarget();
		if (lineDebugFlag)
		{
			bg.Draw();
		}


		if (changeLayerLevelMaxTime[gameStageLevel] <= gameFlame)
		{
			goalBox.Draw();
		}

		//中間演出までのポータル
		if (portalEffect.IsStart() && !portalEffect.IsFinish())
		{
			portalEffect.portalRender.Draw();
		}

		if (stageNum == 0)
		{
			particleRender->Draw();
		}

		stages[stageNum]->SetCamera(0);
		stages[stageNum]->Draw();


		PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, "Enemy");
		//敵の描画処理----------------------------------------------------------------
		for (int enemyType = 0; enemyType < enemies.size(); ++enemyType)
		{
			for (int enemyCount = 0; enemyCount < enemies[enemyType].size(); ++enemyCount)
			{
				//生成されている敵のみ描画処理を通す
				bool enableToUseDataFlag = enemies[enemyType][enemyCount] != nullptr &&
					enemies[enemyType][enemyCount]->GetData()->oprationObjData->initFlag &&
					!enemies[enemyType][enemyCount]->GetData()->outOfStageFlag;
				if (enableToUseDataFlag)
				{
					enemies[enemyType][enemyCount]->Draw();
				}
#ifdef _DEBUG
				if (enableToUseDataFlag && enemies[enemyType][enemyCount]->iOperationData.enableToHitFlag)
				{
					//デバック時のみ当たり判定を描画
					enemyHitBox[enemyType][enemyCount].Draw();
				}
#endif
			}
		}
		PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());


		player.Draw();


		for (int i = 0; i < fireEffect.size(); ++i)
		{
			fireEffect[i].Draw();
		}

		for (int i = 0; i < lightEffect.size(); ++i)
		{
			lightEffect[i].Draw();
		}

		//次ポータルの描画
		if (portalEffect.DrawNextPortal())
		{
			portalEffect.nextPortalRender.Draw();
		}

		if (changeLayerLevelMaxTime[gameStageLevel] <= gameFlame)
		{
			goalBox.lightEffect.Draw();
		}


		for (int i = 0; i < lineLevel.size(); ++i)
		{
			lineLevel[i].Draw();
		}


		//stages[stageNum]->vaporWaveSunRender.Draw();

		for (int i = 0; i < hitEffect.size(); ++i)
		{
			hitEffect[i].Draw();
		}



		renderTarget[stageNum]->Draw();


		//現在の描画
		mainRenderTarget.data.handleData = renderTarget[stageNum]->GetGameRenderTargetHandle();

		//チュートリアル用の描画
		if (tutorial.tutorialFlag)
		{
			mainRenderTarget.data.handleData.handle = tutorial.mainRenderTarget.data.handleData.handle;
		}
		//中間演出の描画
		if (portalEffect.disappearFlag || portalEffect.DrawNextPortal())
		{
			mainRenderTarget.data.handleData = portalEffect.renderTarget->GetGameRenderTargetHandle();
		}

		stringLog.Draw();
		stageUI.Draw();
		logoutWindow->Draw();

		mainRenderTarget.Draw();
		if (tutorial.tutorialFlag)
		{
			tutorial.cursor.Draw();
		}
		else
		{
			cursor.Draw();
		}
	}
	else if (gameOverFlag)
	{
		gameOverTex.data.handleData = gameOverResouceHandle;
		gameOverTex.Draw();
		if (flashFlag)
		{
			pressAButtonTex.Draw();
		}
	}
	else if (gameClearFlag)
	{
		gameOverTex.data.transform.pos = { WIN_X / 2.0f, WIN_Y / 2.0f };
		pressAButtonTex.data.transform.pos = { WIN_X / 2.0f, WIN_Y / 2.0f + 200.0f };
		gameOverTex.data.handleData = gameClearResourceHandle;
		gameOverTex.Draw();
		if (flashFlag)
		{
			pressAButtonTex.Draw();
		}

		playerModel.data.transform.pos.z = 5.0f;
		playerModel.data.transform.pos.y = 2.0f;
		playerModel.data.transform.rotation.y += 1.0f;

		headModel.data.transform.pos.z = 6.0f;
		headModel.data.transform.pos.y = 4.0f;
		headModel.data.transform.pos.x = 1.0f;
		headModel.data.transform.rotation.y += 1.0f;
		headModel.data.transform.rotation.x = 10.0f;

		//playerModel.Draw();
		//headModel.Draw();

	}

	blackTex.Draw();

}

int Game::SceneChange()
{
	if (sceneNum != -1)
	{
		int tmp = sceneNum;
		sceneNum = -1;
		return tmp;
	}
	return SCENE_NONE;
}