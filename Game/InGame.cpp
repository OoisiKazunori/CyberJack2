#include"InGame.h"
#include"../KazLibrary/Buffer/UavViewHandleMgr.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../KazLibrary/Input/ControllerInputManager.h"
#include"Effect/PlayerShotEffectMgr.h"
#include"../KazLibrary/Loader/MeshParticleLoader.h"
#include"PostEffect/Outline.h"
#include"Effect/EnemyDissolveParam.h"

InGame::InGame(const std::array<std::array<ResponeData, KazEnemyHelper::ENEMY_NUM_MAX>, KazEnemyHelper::ENEMY_TYPE_MAX>& arg_responeData, const std::array<std::shared_ptr<IStage>, KazEnemyHelper::STAGE_NUM_MAX>& arg_stageArray, const std::array<KazMath::Color, KazEnemyHelper::STAGE_NUM_MAX>& BACKGROUND_COLOR, const std::array<std::array<KazEnemyHelper::ForceCameraData, 10>, KazEnemyHelper::STAGE_NUM_MAX>& CAMERA_ARRAY) :
	m_stageArray(arg_stageArray), m_responeData(arg_responeData), m_sceneNum(-1)
{
	KazEnemyHelper::GenerateEnemy(m_enemies, m_responeData, enemiesHandle, m_enemyHitBoxArray);

	m_debugFlag = false;

	//m_bloomModelRender = DrawFuncData::SetDrawGLTFIndexMaterialInRayTracingBloomData(*ModelLoader::Instance()->Load("Resource/Player/Kari/", "Player.gltf"), DrawFuncData::GetModelBloomShader());
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

	//“G‚ğ‘S•”¶¬‚µ‚¿‚á‚¤B
	for (int enemyType = 0; enemyType < m_responeData.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < m_responeData[enemyType].size(); ++enemyCount)
		{
			if (!m_enemies[enemyType][enemyCount]) continue;

			m_enemies[enemyType][enemyCount]->OnInit(m_responeData[enemyType][enemyCount].generateData.useMeshPaticleFlag);
			m_enemies[enemyType][enemyCount]->Init(&(m_player.m_transform), m_responeData[enemyType][enemyCount].generateData, false);

			if (m_enemies[enemyType][enemyCount]->GetData()->meshParticleFlag)
			{
				continue;
			}
		}
	}
}

void InGame::Finalize()
{
	m_player.Finalize();
}

void InGame::Input()
{
	KeyBoradInputManager* input = KeyBoradInputManager::Instance();
	ControllerInputManager* cInput = ControllerInputManager::Instance();
	if (input->InputTrigger(DIK_ESCAPE))
	{
		Init(false);
	}

	m_player.Input();


	bool upFlag = false;
	bool downFlag = false;
	bool leftFlag = false;
	bool rightFlag = false;
	bool doneFlag = false;
	bool releaseFlag = false;

	const int DEAD_ZONE = 3000;
	if (cInput->InputState(XINPUT_GAMEPAD_A))
	{
		doneFlag = true;
	}
	if (cInput->InputRelease(XINPUT_GAMEPAD_A))
	{
		releaseFlag = true;
	}
	if (cInput->InputStickState(LEFT_STICK, UP_SIDE, DEAD_ZONE))
	{
		upFlag = true;
	}
	if (cInput->InputStickState(LEFT_STICK, DOWN_SIDE, DEAD_ZONE))
	{
		downFlag = true;
	}
	if (cInput->InputStickState(LEFT_STICK, LEFT_SIDE, DEAD_ZONE))
	{
		leftFlag = true;
	}
	if (cInput->InputStickState(LEFT_STICK, RIGHT_SIDE, DEAD_ZONE))
	{
		rightFlag = true;
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
		joyStick
	);
}

void InGame::Update()
{
#pragma region “G‚Ì¶¬ˆ—
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

				if (m_enemies[enemyType][enemyCount]->GetData()->meshParticleFlag)
				{
					continue;
				}
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
					m_cursor.Hit(enemyData->hitBox.center);

					m_enemies[enemyType][enemyCount]->Hit();
					//PlayerShotEffectMgr::Instance()->Generate(m_enemies[enemyType][enemyCount]);

					//stringLog.WriteLog(enemies[enemyType][enemyCount]->GetData()->oprationObjData->name, LOG_FONT_SIZE);

					////ü‚Ì”­“®ˆ—
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

	//“G‚ÌXVˆ—----------------------------------------------
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
	//“G‚ÌXVˆ—----------------------------------------------


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
					//UŒ‚‚ğ“–‚Ä‚é“GˆÈŠO‚ÌQÆ‚à•Û‘¶‚µ‚Ä“n‚·B
					std::array<std::shared_ptr<IEnemy>, 3> refOtherEnemy;
					int counter = 0;
					for (int index = 0; index < 4; ++index) {
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
		/*for (int enemyType = 0; enemyType < m_responeData.size(); ++enemyType)
		{
			for (int enemyCount = 0; enemyCount < m_responeData[enemyType].size(); ++enemyCount)
			{
				if (!m_enemies[enemyType][enemyCount]) continue;

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
		m_lightIndex = 0;*/
	}
	else
	{
		m_gameSpeed = 1;
	}

	PlayerShotEffectMgr::Instance()->Update(&m_camera.GetEyePos(), m_stageArray[m_gameStageLevel]->hitFlag, &m_lightIndex);

	//m_gameFlame += m_gameSpeed;

	//ƒQ[ƒ€ƒ‹[ƒv‚Ì‰Šú‰»
	//if (KazMath::ConvertSecondToFlame(15) <= m_gameFlame)
	if (720 <= m_gameFlame)
	{
		m_gameFlame = 0;
	}
}

void InGame::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{

	PlayerShotEffectMgr::Instance()->Draw(arg_rasterize, arg_blasVec);

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

#ifdef _DEBUG
	if (m_debugFlag)
	{
		//m_rail.DebugDraw(arg_rasterize);
	}
#endif

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
