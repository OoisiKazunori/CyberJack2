#include"InGame.h"
#include"../KazLibrary/Buffer/UavViewHandleMgr.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../KazLibrary/Input/ControllerInputManager.h"
#include"Effect/PlayerShotEffectMgr.h"
#include"../KazLibrary/Loader/MeshParticleLoader.h"
#include"PostEffect/Outline.h"
#include"Effect/EnemyDissolveParam.h"
#include"Effect/ShockWave.h"
#include"Effect/SeaEffect.h"
#include"Effect/ShakeMgr.h"

InGame::InGame(const std::array<std::array<ResponeData, KazEnemyHelper::ENEMY_NUM_MAX>, KazEnemyHelper::ENEMY_TYPE_MAX>& arg_responeData, const std::array<std::shared_ptr<IStage>, KazEnemyHelper::STAGE_NUM_MAX>& arg_stageArray, const std::array<KazMath::Color, KazEnemyHelper::STAGE_NUM_MAX>& BACKGROUND_COLOR, const std::array<std::array<KazEnemyHelper::ForceCameraData, 10>, KazEnemyHelper::STAGE_NUM_MAX>& CAMERA_ARRAY) :
	m_stageArray(arg_stageArray), m_responeData(arg_responeData), m_sceneNum(-1)
{
	KazEnemyHelper::GenerateEnemy(m_enemies, m_responeData, enemiesHandle, m_enemyHitBoxArray);

	m_debugFlag = false;

	m_lockonSE = SoundManager::Instance()->SoundLoadWave("Resource/Sound/lockon.wav");
	m_lockonSE.volume = 0.01f;

	//Infomation
	DrawFuncData::PipelineGenerateData pipelineData = DrawFuncData::GetSpriteAlphaShader();
	pipelineData.desc.DepthStencilState.DepthEnable = true;							//深度テストを行う
	pipelineData.desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込み許可
	pipelineData.desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;		//小さければOK
	pipelineData.desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;							//深度値フォーマット
	m_guideRender = DrawFuncData::SetSpriteAlphaData(pipelineData);
	m_infomationTex = TextureResourceMgr::Instance()->LoadGraphBuffer("Resource/UI/Guide/Infomation.png");

	//操作ガイドのUI
	m_guideUI = DrawFuncData::SetSpriteAlphaData(DrawFuncData::GetSpriteAlphaShader());
	m_guideTex = TextureResourceMgr::Instance()->LoadGraphBuffer("Resource/UI/Guide/Guide.png");
	//m_bloomModelRender = DrawFuncData::SetDrawGLTFIndexMaterialInRayTracingBloomData(*ModelLoader::Instance()->Load("Resource/Player/Kari/", "Player.gltf"), DrawFuncData::GetModelBloomShader());

	//for (int i = 0; i < m_sponzaModel.size(); ++i)
	//{
	//	m_sponzaModel[i] = DrawFuncData::SetDefferdRenderingModel(ModelLoader::Instance()->Load("Resource/Test/glTF/Sponza/", "Sponza.gltf"));
	//}


	m_hitSE = SoundManager::Instance()->SoundLoadWave("Resource/Sound/hit.wav");
	m_hitSE.volume = 0.01f;

	//右固め
	m_responePatternArray[0][0] = { 50.0f,25.0f,50.0f };
	m_responePatternArray[0][1] = { 30.0f,35.0f,75.0f };
	m_responePatternArray[0][2] = { 25.0f,55.0f,100.0f };
	//左固め
	m_responePatternArray[0][3] = { -50.0f,25.0f,50.0f };
	m_responePatternArray[0][4] = { -30.0f,35.0f,75.0f };
	m_responePatternArray[0][6] = { -25.0f,55.0f,100.0f };
	//真ん中
	m_responePatternArray[0][5] = { 0.0f,25.0f,50.0f };
	m_responePatternArray[0][7] = { 0.0f,60.0f,100.0f };


	//右固め
	m_responePatternArray[1][0] = { 50.0f,25.0f,50.0f };
	m_responePatternArray[1][1] = { 50.0f,50.0f,75.0f };
	m_responePatternArray[1][2] = { 50.0f,25.0f,100.0f };
	//左固め
	m_responePatternArray[1][3] = { -50.0f,25.0f,50.0f };
	m_responePatternArray[1][4] = { -50.0f,50.0f,75.0f };
	m_responePatternArray[1][5] = { -30.0f,25.0f,100.0f };
	//真ん中
	m_responePatternArray[1][6] = { 0.0f,25.0f,50.0f };
	m_responePatternArray[1][7] = { 0.0f,70.0f,150.0f };


	//右固め
	m_responePatternArray[2][0] = { 25.0f,25.0f,50.0f };
	m_responePatternArray[2][1] = { 45.0f,55.0f,75.0f };
	m_responePatternArray[2][2] = { 65.0f,85.0f,95.0f };
	//左固め
	m_responePatternArray[2][3] = { -25.0f,25.0f,50.0f };
	m_responePatternArray[2][4] = { -45.0f,55.0f,75.0f };
	m_responePatternArray[2][5] = { -65.0f,85.0f,95.0f };
	//真ん中
	m_responePatternArray[2][6] = { 0.0f, 25.0f,50.0f };
	m_responePatternArray[2][7] = { 0.0f, 75.0f,105.0f };
}

void InGame::Init(bool SKIP_FLAG)
{
	m_rail.Init();
	m_player.Init(KazMath::Vec3<float>(0.0f, 0.0f, 30.0f));
	m_camera.Init();
	m_gameFlame = 0;
	m_gameSpeed = 1;
	m_notMoveTimer = 0;
	m_isEnemyNotMoveFlag = false;
	m_sceneNum = -1;
	m_cursor.Init();

	m_butterflyEnemyRespawnDelay = 0;


	m_responePatternIndex = KazMath::Rand<size_t>(m_responePatternArray.size() - 1, 0);
	if (m_responePatternArray.size() <= m_responePatternIndex)
	{
		m_responePatternIndex = 0;
	}

	std::array<bool, 10>flagArray;
	for (auto& obj : flagArray)
	{
		obj = false;
	}

	//敵を全部生成しちゃう。
	for (int enemyType = 0; enemyType < m_responeData.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < m_responeData[enemyType].size(); ++enemyCount)
		{
			if (!m_enemies[enemyType][enemyCount]) continue;

			size_t index(GetRespownPattern(flagArray));
			//出現場所をランダムにする。
			m_responeData[enemyType][enemyCount].generateData.initPos = m_responePatternArray[m_responePatternIndex][index];

			m_enemies[enemyType][enemyCount]->OnInit(m_responeData[enemyType][enemyCount].generateData.useMeshPaticleFlag);
			m_enemies[enemyType][enemyCount]->Init(&(m_player.m_transform), m_responeData[enemyType][enemyCount].generateData, false);

			if (m_enemies[enemyType][enemyCount]->GetData()->meshParticleFlag)
			{
				continue;
			}
		}
	}
	//SoundManager::Instance()->PlaySoundMem(m_bgmHandle, 10, true);
	m_guideTimer = 0;
	m_guideAlphaTimer = 0.0f;
}

void InGame::Finalize()
{
	m_player.Finalize();
}

void InGame::Input()
{
	KeyBoradInputManager* input = KeyBoradInputManager::Instance();
	ControllerInputManager* cInput = ControllerInputManager::Instance();

	m_player.Input();


	bool upFlag = false;
	bool downFlag = false;
	bool leftFlag = false;
	bool rightFlag = false;
	bool doneFlag = false;
	bool releaseFlag = false;

	KazMath::Vec2<float>mouseVel(input->GetMouseVel().x, input->GetMouseVel().y);

	const int DEAD_ZONE = 3000;
	if (cInput->InputState(XINPUT_GAMEPAD_A) || input->MouseInputState(MOUSE_INPUT_LEFT))
	{
		doneFlag = true;
	}
	if (cInput->InputRelease(XINPUT_GAMEPAD_A) || input->MouseInputRelease(MOUSE_INPUT_LEFT))
	{
		releaseFlag = true;
	}

	KazMath::Vec2<float>dir = mouseVel - m_prevMouseVel;

	if (cInput->InputStickState(LEFT_STICK, UP_SIDE, DEAD_ZONE) || std::signbit(dir.y))
	{
		upFlag = true;
	}
	if (cInput->InputStickState(LEFT_STICK, DOWN_SIDE, DEAD_ZONE) || !std::signbit(dir.y))
	{
		downFlag = true;
	}
	if (cInput->InputStickState(LEFT_STICK, LEFT_SIDE, DEAD_ZONE) || std::signbit(dir.x))
	{
		leftFlag = true;
	}
	if (cInput->InputStickState(LEFT_STICK, RIGHT_SIDE, DEAD_ZONE) || !std::signbit(dir.x))
	{
		rightFlag = true;
	}

	if (input->InputTrigger(DIK_O))
	{
		bool debug = false;
	}

	KazMath::Vec2<float> joyStick;
	joyStick.x = cInput->GetJoyStickLXNum(0) / 32767.0f;
	joyStick.y = cInput->GetJoyStickLYNum(0) / 32767.0f;

	m_cursor.Input
	(
		upFlag,
		downFlag,
		leftFlag,
		rightFlag,
		doneFlag,
		releaseFlag,
		joyStick,
		mouseVel
	);
	m_prevMouseVel = mouseVel;
}

void InGame::Update()
{
#pragma region 敵の生成処理
	KazEnemyHelper::AddEnemy(m_enemies, m_responeData, addEnemiesHandle, m_gameFlame, m_gameStageLevel);


	for (int enemyType = 0; enemyType < m_responeData.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < m_responeData[enemyType].size(); ++enemyCount)
		{
			if (!m_enemies[enemyType][enemyCount]) continue;

			bool enableToUseThisDataFlag = m_responeData[enemyType][enemyCount].layerLevel != -1;
			//bool readyToStartFlag = m_responeData[enemyType][enemyCount].flame == m_gameFlame &&
			//	m_responeData[enemyType][enemyCount].layerLevel == m_gameStageLevel;
			bool readyToStartFlag = m_enemies[enemyType][enemyCount]->m_canSpawn;

			if (enableToUseThisDataFlag && readyToStartFlag && m_enemies[enemyType][enemyCount] != nullptr && !m_enemies[enemyType][enemyCount]->GetData()->oprationObjData->initFlag)
			{
				m_enemies[enemyType][enemyCount]->OnInit(m_responeData[enemyType][enemyCount].generateData.useMeshPaticleFlag);
				m_enemies[enemyType][enemyCount]->Init(&(m_player.m_transform), m_responeData[enemyType][enemyCount].generateData, false);
			}
		}
	}
#pragma endregion


#pragma region ?G???b?N?I??
	for (int enemyType = 0; enemyType < m_enemies.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < m_enemies[enemyType].size(); ++enemyCount)
		{
			bool enableToUseDataFlag = m_enemies[enemyType][enemyCount] != nullptr && m_enemies[enemyType][enemyCount]->GetData()->oprationObjData->initFlag;
			if (enableToUseDataFlag)
			{
				EnemyData* enemyData = m_enemies[enemyType][enemyCount]->GetData().get();

				bool enableToLockOnNumFlag = m_cursor.LockOn();
				bool enableToLockOnEnemyFlag = m_enemies[enemyType][enemyCount]->IsAlive() && !m_enemies[enemyType][enemyCount]->LockedOrNot();
				bool hitFlag = CollisionManager::Instance()->CheckRayAndSphere(m_cursor.hitBox, enemyData->hitBox);
				if (hitFlag &&
					enableToLockOnNumFlag &&
					enableToLockOnEnemyFlag &&
					!m_cursor.releaseFlag &&
					m_enemies[enemyType][enemyCount]->m_canLockOn)
				{

					SoundManager::Instance()->SoundPlayerWave(m_lockonSE, 0);

					m_cursor.Hit(enemyData->hitBox.center);

					m_enemies[enemyType][enemyCount]->Hit();
					//PlayerShotEffectMgr::Instance()->Generate(m_enemies[enemyType][enemyCount]);

					//stringLog.WriteLog(enemies[enemyType][enemyCount]->GetData()->oprationObjData->name, LOG_FONT_SIZE);

					////線の発動処理
					//for (int i = 0; i < lineEffectArrayData.size(); ++i)
					//{
					//	if (!lineEffectArrayData[i].usedFlag)
					//	{
					//		lineLevel[i].Attack(lineEffectArrayData[i].startPos, *enemyData->hitBox.center);
					//		lineEffectArrayData[i].usedFlag = true;
					//		lineEffectArrayData[i].lineIndex = i;
					//		lineEffectArrayData[i].enemyTypeIndex = enemyType;
					//		lineEffectArrayData[i].enemyIndex = enemyCount;
					//		break;
					//	}
					//}
				}
			}
		}
	}


	for (int enemyType = 0; enemyType < m_enemies.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < m_enemies[enemyType].size(); ++enemyCount)
		{
			bool enableToUseDataFlag = m_enemies[enemyType][enemyCount] != nullptr && m_enemies[enemyType][enemyCount]->GetData()->oprationObjData->initFlag;
			if (enableToUseDataFlag)
			{
				EnemyData* enemyData = m_enemies[enemyType][enemyCount]->GetData().get();

				/*for (int i = 0; i < lineEffectArrayData.size(); ++i)
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
				}*/
			}
		}
	}


	//for (int enemyType = 0; enemyType < enemies.size(); ++enemyType)
	//{
	//	for (int enemyCount = 0; enemyCount < enemies[enemyType].size(); ++enemyCount)
	//	{
	//		bool enableToUseDataFlag = enemies[enemyType][enemyCount] != nullptr && enemies[enemyType][enemyCount]->GetData()->oprationObjData->initFlag;
	//		if (enableToUseDataFlag)
	//		{
	//			EnemyData* enemyData = enemies[enemyType][enemyCount]->GetData().get();

	//			for (int i = 0; i < lineEffectArrayData.size(); ++i)
	//			{
	//				bool sameEnemyFlag = lineEffectArrayData[i].enemyTypeIndex == enemyType && lineEffectArrayData[i].enemyIndex == enemyCount;
	//				bool releaseFlag = enemyData->outOfStageFlag || enemyData->timer <= 0;

	//				if (lineEffectArrayData[i].usedFlag && sameEnemyFlag && releaseFlag)
	//				{
	//					int lineIndex = lineEffectArrayData[i].lineIndex;
	//					lineLevel[lineIndex].Release();
	//					cursor.SubCount(1);
	//					break;
	//				}
	//			}
	//		}
	//	}
	//}



#pragma region ??????b?N?I?????

	//for (int i = 0; i < lineEffectArrayData.size(); ++i)
	//{
	//	if (lineEffectArrayData[i].usedFlag)
	//	{
	//		int lineIndex = lineEffectArrayData[i].lineIndex;
	//		int enemyTypeIndex = lineEffectArrayData[i].enemyTypeIndex;
	//		int enemyIndex = lineEffectArrayData[i].enemyIndex;
	//		int eventIndex = lineEffectArrayData[i].eventType;

	//		if (eventIndex != -1)
	//		{
	//			break;
	//		}

	//		if (lineLevel[lineIndex].lineReachObjFlag && !lineLevel[lineIndex].alreadyReachedFlag && enemies[enemyTypeIndex][enemyIndex]->IsAlive())
	//		{
	//			SoundManager::Instance()->PlaySoundMem(damageSoundHandle, 1);
	//			lineLevel[lineIndex].alreadyReachedFlag = true;
	//		}
	//		else if (lineLevel[lineIndex].lineReachObjFlag && !enemies[enemyTypeIndex][enemyIndex]->IsAlive() && !lineEffectArrayData[i].hitFlag)
	//		{
	//			enemies[enemyTypeIndex][enemyIndex]->Dead();
	//			lineEffectArrayData[i].hitFlag = true;

	//			for (int hitEffectIndex = 0; hitEffectIndex < hitEffect.size(); ++hitEffectIndex)
	//			{
	//				if (!hitEffect[hitEffectIndex].IsAlive())
	//				{
	//					hitEffect[hitEffectIndex].Start(*enemies[enemyTypeIndex][enemyIndex]->GetData()->hitBox.center);
	//					break;
	//				}
	//			}

	//			stages[stageNum]->hitFlag = true;
	//		}
	//	}
	//}

	//for (int i = 0; i < lineEffectArrayData.size(); ++i)
	//{
	//	if (lineEffectArrayData[i].usedFlag)
	//	{
	//		int lineIndex = lineEffectArrayData[i].lineIndex;
	//		if (lineLevel[lineIndex].allFinishFlag)
	//		{
	//			lineEffectArrayData[i].Reset();
	//		}
	//	}
	//}

	//if (cursor.Release() && cursor.GetCount() != 0)
	//{
	//	const std::string lLog("Fire_[" + std::to_string(cursor.GetCount()) + "x]");
	//	stringLog.WriteLog(lLog, LOG_FONT_SIZE);
	//}

#pragma endregion


#pragma endregion

	//敵の更新処理----------------------------------------------
	for (int enemyType = 0; enemyType < m_enemies.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < m_enemies[enemyType].size(); ++enemyCount)
		{
			bool enableToUseDataFlag = m_enemies[enemyType][enemyCount] != nullptr && m_enemies[enemyType][enemyCount]->GetData()->oprationObjData->enableToHitFlag;
			if (enableToUseDataFlag)
			{
				if (m_enemies[enemyType][enemyCount]->GetData()->timer <= 0)
				{
					m_player.Hit();

					//enemies[enemyType][enemyCount]->Dead();
				}
			}
		}
	}
	//敵の更新処理----------------------------------------------


	m_isEnemyNotMoveFlag = true;
	for (int enemyType = 0; enemyType < m_enemies.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < m_enemies[enemyType].size(); ++enemyCount)
		{
			bool enableToUseDataFlag =
				m_enemies[enemyType][enemyCount] != nullptr &&
				m_enemies[enemyType][enemyCount]->GetData()->oprationObjData->initFlag &&
				!m_enemies[enemyType][enemyCount]->GetData()->outOfStageFlag;
			if (enableToUseDataFlag)
			{
				if (m_enemies[enemyType][enemyCount]->GetData()->oprationObjData->enableToHitFlag)
				{
					m_enemies[enemyType][enemyCount]->SetLight(m_cursor.hitBox.dir, m_enemies[enemyType][enemyCount]->GetData()->objFlag);
				}
				if (m_enemies[enemyType][enemyCount]->GetData()->oprationObjData->rockOnNum <= 0 &&
					m_cursor.Release() &&
					!m_enemies[enemyType][enemyCount]->m_isBeingShot)
				{
					//攻撃を当てる敵以外の参照も保存して渡す。
					std::array<std::shared_ptr<IEnemy>, 7> refOtherEnemy;
					int counter = 0;
					for (int index = 0; index < 8; ++index) {
						if (m_enemies[enemyType][enemyCount] == m_enemies[enemyType][index]) continue;

						refOtherEnemy[counter] = m_enemies[enemyType][index];
						++counter;
					}

					PlayerShotEffectMgr::Instance()->Generate(m_enemies[enemyType][enemyCount], refOtherEnemy);
					m_enemies[enemyType][enemyCount]->m_isBeingShot = true;
				}
			}
			if (m_enemies[enemyType][enemyCount])
			{
				m_enemies[enemyType][enemyCount]->Update();
			}


			if (enableToUseDataFlag && m_enemies[enemyType][enemyCount]->GetData()->oprationObjData->enableToHitFlag)
			{
				m_isEnemyNotMoveFlag = false;
			}
		}
	}



	//bool gameClearFlag = !m_debugFlag && m_rail.IsEnd();
	//if (gameClearFlag || KeyBoradInputManager::Instance()->InputTrigger(DIK_3))
	//{
	//	m_sceneNum = 2;
	//}
	//if (!m_player.IsAlive() || KeyBoradInputManager::Instance()->InputTrigger(DIK_2))
	//{
	//	m_sceneNum = 3;
	//}


	m_rail.Update();

	//if (m_gameFlame % (60 * 3) == 0)
	//{
	//	if (m_lightIndex < m_stageArray[m_gameStageLevel]->hitFlag.size())
	//	{
	//		m_stageArray[m_gameStageLevel]->hitFlag[m_lightIndex] = true;
	//	}
	//	++m_lightIndex;
	//}

	m_stageArray[m_gameStageLevel]->playerPos = m_rail.GetPosition();
	m_stageArray[m_gameStageLevel]->Update();

	m_player.Update();
	//m_player.pos = m_rail.GetPosition();
	m_player.pos = KazMath::Vec3<float>(0, 20, 0);
	m_cursor.Update();
	m_camera.Update(m_cursor.GetValue(), &m_player.pos, m_player.m_transform.rotation, false);

	if (!m_debugFlag)
	{
		CameraMgr::Instance()->Camera(m_camera.GetEyePos(), m_camera.GetTargetPos(), { 0.0f,1.0f,0.0f });
	}
	else
	{
		m_debugCamera.Update();
	}


	if (m_isEnemyNotMoveFlag)
	{
		++m_notMoveTimer;
	}
	else
	{
		m_notMoveTimer = 0;
	}

	if (180 == m_notMoveTimer)
	{
		std::array<bool, 10>flagArray;
		for (auto& obj : flagArray)
		{
			obj = false;
		}
		++m_responePatternIndex;
		if (m_responePatternArray.size() <= m_responePatternIndex)
		{
			m_responePatternIndex = 0;
		}

		for (int enemyType = 0; enemyType < m_responeData.size(); ++enemyType)
		{
			for (int enemyCount = 0; enemyCount < m_responeData[enemyType].size(); ++enemyCount)
			{
				if (!m_enemies[enemyType][enemyCount]) continue;

				size_t index(GetRespownPattern(flagArray));
				//出現場所をランダムにする。
				m_responeData[enemyType][enemyCount].generateData.initPos = m_responePatternArray[m_responePatternIndex][index];

				m_enemies[enemyType][enemyCount]->OnInit(m_responeData[enemyType][enemyCount].generateData.useMeshPaticleFlag);
				m_enemies[enemyType][enemyCount]->Init(&(m_player.m_transform), m_responeData[enemyType][enemyCount].generateData, false);

				if (m_enemies[enemyType][enemyCount]->GetData()->meshParticleFlag)
				{
					continue;
				}
			}
		}

		for (int i = 0; i < m_stageArray[m_gameStageLevel]->hitFlag.size(); ++i)
		{
			m_stageArray[m_gameStageLevel]->hitFlag[i] = false;
		}
		m_lightIndex = 0;
	}
	else
	{
		m_gameSpeed = 1;
	}

	PlayerShotEffectMgr::Instance()->Update(&m_camera.GetEyePos(), m_stageArray[m_gameStageLevel]->hitFlag, &m_lightIndex);

	//m_gameFlame += m_gameSpeed;

	//ゲームループの初期化
	//if (KazMath::ConvertSecondToFlame(15) <= m_gameFlame)
	if (720 <= m_gameFlame)
	{
		m_gameFlame = 0;
	}


	//無理やり衝撃波を出す。
	if (KeyBoradInputManager::Instance()->InputTrigger(DIK_RETURN) || ControllerInputManager::Instance()->InputTrigger(XINPUT_GAMEPAD_B)) {

		//ランダムで敵を選択して衝撃波を出す。
		int randomEnemy = KazMath::Rand(0, 7);

		ShakeMgr::Instance()->m_shakeAmount = 0.4f;
		SeaEffect::Instance()->m_isSeaEffect = true;
		ShockWave::Instance()->m_shockWave[randomEnemy].m_isActive = true;
		ShockWave::Instance()->m_shockWave[randomEnemy].m_power = 1.0f;
		ShockWave::Instance()->m_shockWave[randomEnemy].m_radius = 0.0f;
		m_enemies[ENEMY_TYPE_VIRUS][randomEnemy]->m_shockWaveTimer = 0;
		SoundManager::Instance()->SoundPlayerWave(m_hitSE, 0);
	}

}

void InGame::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{

	PlayerShotEffectMgr::Instance()->Draw(arg_rasterize, arg_blasVec);

	{
		++m_guideTimer;
		float alpha = 255.0f;
		if (60 * 3 < m_guideTimer)
		{
			Rate(&m_guideAlphaTimer, 0.01f, 1.0f);
			alpha = 255.0f - EasingMaker(Out, Cubic, m_guideAlphaTimer) * 255.0f;
		}
		KazMath::Color color = KazMath::Color(255, 255, 255, static_cast<int>(alpha));
		KazMath::Transform2D transform;
		transform.pos = KazMath::Vec2<float>(1280.0f / 2.0f, 720.0f / 2.0f);
		transform.scale = KazMath::Vec2<float>(1280.0f, 720.0f);
		DrawFunc::DrawTextureIn2D(m_guideRender, transform, m_infomationTex, color);
		arg_rasterize.ObjectRender(m_guideRender);
	}


	PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, "Enemy");
	for (int enemyType = 0; enemyType < m_enemies.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < m_enemies[enemyType].size(); ++enemyCount)
		{
			bool enableToUseDataFlag = m_enemies[enemyType][enemyCount] != nullptr;
			if (enableToUseDataFlag)
			{
				m_enemies[enemyType][enemyCount]->Draw(arg_rasterize, arg_blasVec);
			}
		}
	}
	PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());
	m_player.Draw(arg_rasterize, arg_blasVec);

	if (!m_debugFlag)
	{
		m_cursor.Draw(arg_rasterize);
	}


	{
		KazMath::Color color = KazMath::Color(255, 255, 255, 255);
		KazMath::Transform2D transform;
		transform.scale = KazMath::Vec2<float>(338.0f, 82.0f);
		transform.pos = KazMath::Vec2<float>(1280.0f - transform.scale.x / 2.0f, 720.0f - transform.scale.y / 2.0f);
		DrawFunc::DrawTextureIn2D(m_guideUI, transform, m_guideTex, color);
		arg_rasterize.ObjectRender(m_guideUI);
	}

	m_stageArray[m_gameStageLevel]->Draw(arg_rasterize, arg_blasVec);

	//KazMath::Transform3D transform(KazMath::Vec3<float>(10.0f, 10.0f, 10.0f));
	//DrawFunc::DrawModelInRaytracing(m_bloomModelRender, transform, DrawFunc::NONE, KazMath::Color(255, 0, 0, 255));
	//for (auto& obj : m_bloomModelRender.m_raytracingData.m_blas)
	//{
	//	arg_blasVec.Add(obj, transform.GetMat());
	//}
	//arg_rasterize.ObjectRender(m_bloomModelRender);

	//ImGui::Begin("Game");
	//ImGui::Checkbox("Debug", &m_debugFlag);
	//ImGui::End();

	//for (int i = 0; i < m_sponzaModel.size(); ++i)
	//{
	//	arg_rasterize.ObjectRender(m_sponzaModel[i]);
	//}
}

int InGame::SceneChange()
{
	if (m_sceneNum != -1)
	{
		int tmp = m_sceneNum;
		m_sceneNum = -1;
		return tmp;
	}
	return SCENE_NONE;
}

void InGame::BufferStatesTransition(ID3D12Resource* arg_resource, D3D12_RESOURCE_STATES arg_before, D3D12_RESOURCE_STATES arg_after)
{
	D3D12_RESOURCE_BARRIER barriers[] = {
	CD3DX12_RESOURCE_BARRIER::Transition(
		arg_resource,
		arg_before,
		arg_after),
	};
	DirectX12CmdList::Instance()->cmdList->ResourceBarrier(_countof(barriers), barriers);
}
