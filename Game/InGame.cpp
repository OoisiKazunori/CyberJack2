#include "InGame.h"
#include"../KazLibrary/Buffer/UavViewHandleMgr.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../KazLibrary/Input/ControllerInputManager.h"
#include"../KazLibrary/Loader/MeshParticleLoader.h"

InGame::InGame(const std::array<std::array<ResponeData, KazEnemyHelper::ENEMY_NUM_MAX>, KazEnemyHelper::ENEMY_TYPE_MAX>& arg_responeData, const std::array<std::shared_ptr<IStage>, KazEnemyHelper::STAGE_NUM_MAX>& arg_stageArray, const std::array<KazMath::Color, KazEnemyHelper::STAGE_NUM_MAX>& BACKGROUND_COLOR, const std::array<std::array<KazEnemyHelper::ForceCameraData, 10>, KazEnemyHelper::STAGE_NUM_MAX>& CAMERA_ARRAY) :
	m_stageArray(arg_stageArray), m_responeData(arg_responeData), m_sceneNum(-1)
{
	KazEnemyHelper::GenerateEnemy(m_enemies, m_responeData, enemiesHandle, m_enemyHitBoxArray);

	m_debugFlag = false;

	struct OutputData
	{
		DirectX::XMMATRIX mat;
		DirectX::XMFLOAT4 color;
	};
	m_particleRender = KazBufferHelper::SetGPUBufferData(sizeof(OutputData) * 3000000);
	m_particleRender.structureSize = sizeof(OutputData);
	m_particleRender.elementNum = 3000000;
	m_particleRender.GenerateCounterBuffer();
	m_particleRender.CreateUAVView();

	m_particleColorRender = KazBufferHelper::SetGPUBufferData(sizeof(DirectX::XMFLOAT4) * 3000000);
	m_meshParticleRender = std::make_unique<InstanceMeshParticle>(m_particleRender, m_particleColorRender);

	m_executeIndirect = DrawFuncData::SetExecuteIndirect(
		DrawFuncData::GetBasicShader(),
		m_particleRender.bufferWrapper->GetBuffer()->GetGPUVirtualAddress(),
		3000000
	);

	m_executeIndirect.extraBufferArray.emplace_back(m_particleRender);
	m_executeIndirect.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_executeIndirect.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
	m_modelRender = DrawFuncData::SetDrawGLTFIndexMaterialData(*ModelLoader::Instance()->Load("Resource/Test/glTF/Box/", "BoxTextured.gltf"), DrawFuncData::GetModelShader());
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


	InitMeshParticleData initMeshParticleData(MeshParticleLoader::Instance()->LoadMesh("Resource/Test/glTF/Box/", "BoxTextured.gltf", &m_motherMat, { 70,100,12 }, -1));
	initMeshParticleData.alpha = &m_alpha;
	float scale = 0.01f;
	initMeshParticleData.particleScale = { scale,scale,scale };
	m_alpha = 1.0f;
	m_meshParticleRender->AddMeshData(initMeshParticleData);

	m_motherMat = KazMath::Transform3D({ 0.0f,0.0f,0.0f }, { 5.0f,5.0f,5.0f }, { 0.0f,0.0f,0.0f }).GetMat();
	m_meshParticleRender->Init();
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

	if (KeyBoradInputManager::Instance()->MouseInputState(MOUSE_INPUT_LEFT))
	{
		m_meshParticleRender->InitCompute();
	}
	m_particleRender.counterWrapper->CopyBuffer(m_meshParticleRender->copyBuffer.GetBuffer());
	m_meshParticleRender->Compute();

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
#pragma region ��������
	//�G��ǉ��ŏ��������鏈��----------------------------------------------------------------
	KazEnemyHelper::AddEnemy(m_enemies, m_responeData, addEnemiesHandle, m_gameFlame, m_gameStageLevel);
	//�G��ǉ��ŏ��������鏈��----------------------------------------------------------------


	//�G���ǂ̃^�C�~���O�ŏ��������鏈��----------------------------------------------------------------
	for (int enemyType = 0; enemyType < m_responeData.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < m_responeData[enemyType].size(); ++enemyCount)
		{
			bool enableToUseThisDataFlag = m_responeData[enemyType][enemyCount].layerLevel != -1;
			bool readyToStartFlag = m_responeData[enemyType][enemyCount].flame <= m_gameFlame &&
				m_responeData[enemyType][enemyCount].layerLevel == m_gameStageLevel;

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
	//�G���ǂ̃^�C�~���O�ŏ��������鏈��----------------------------------------------------------------
#pragma endregion


#pragma region �G���b�N�I��
	for (int enemyType = 0; enemyType < m_enemies.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < m_enemies[enemyType].size(); ++enemyCount)
		{
			//��������Ă���A���������Ă���G�̂ݍX�V������ʂ�
			bool enableToUseDataFlag = m_enemies[enemyType][enemyCount] != nullptr && m_enemies[enemyType][enemyCount]->GetData()->oprationObjData->initFlag;
			if (enableToUseDataFlag)
			{
				//�G�̃f�[�^�̃|�C���^����
				EnemyData* enemyData = m_enemies[enemyType][enemyCount]->GetData().get();

				//���b�N�I������
				bool enableToLockOnNumFlag = m_cursor.LockOn();
				bool enableToLockOnEnemyFlag = m_enemies[enemyType][enemyCount]->IsAlive() && !m_enemies[enemyType][enemyCount]->LockedOrNot();
				bool hitFlag = CollisionManager::Instance()->CheckRayAndSphere(m_cursor.hitBox, enemyData->hitBox);
				if (hitFlag &&
					enableToLockOnNumFlag &&
					enableToLockOnEnemyFlag &&
					!m_cursor.releaseFlag)
				{

					//�J�[�\���̃J�E���g���𑝂₷
					m_cursor.Hit(enemyData->hitBox.center);
					//�G������������������
					m_enemies[enemyType][enemyCount]->Hit();

					//stringLog.WriteLog(enemies[enemyType][enemyCount]->GetData()->oprationObjData->name, LOG_FONT_SIZE);

					////�����o��������ۂɂǂ̔z����g�p���邩���߂�
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
			//��������Ă���A���������Ă���G�̂ݍX�V������ʂ�
			bool enableToUseDataFlag = m_enemies[enemyType][enemyCount] != nullptr && m_enemies[enemyType][enemyCount]->GetData()->oprationObjData->initFlag;
			if (enableToUseDataFlag)
			{
				//�G�̃f�[�^�̃|�C���^����
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
	//		//��������Ă���A���������Ă���G�̂ݍX�V������ʂ�
	//		bool enableToUseDataFlag = enemies[enemyType][enemyCount] != nullptr && enemies[enemyType][enemyCount]->GetData()->oprationObjData->initFlag;
	//		if (enableToUseDataFlag)
	//		{
	//			//�G�̃f�[�^�̃|�C���^����
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



#pragma region ���̃��b�N�I�����

	//�������ǂ蒅������G�����S������
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

	//		//���o�����킹�ă_���[�W�Ǝ��S�����
	//		if (lineLevel[lineIndex].lineReachObjFlag && !lineLevel[lineIndex].alreadyReachedFlag && enemies[enemyTypeIndex][enemyIndex]->IsAlive())
	//		{
	//			SoundManager::Instance()->PlaySoundMem(damageSoundHandle, 1);
	//			lineLevel[lineIndex].alreadyReachedFlag = true;
	//		}
	//		else if (lineLevel[lineIndex].lineReachObjFlag && !enemies[enemyTypeIndex][enemyIndex]->IsAlive() && !lineEffectArrayData[i].hitFlag)
	//		{
	//			enemies[enemyTypeIndex][enemyIndex]->Dead();
	//			lineEffectArrayData[i].hitFlag = true;

	//			//�q�b�g���̉~���o
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

	////�����o���I������ۂɃ����N�̃f�[�^�����Z�b�g����
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

	//�U��----------------------------------------------
	for (int enemyType = 0; enemyType < m_enemies.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < m_enemies[enemyType].size(); ++enemyCount)
		{
			//��������Ă���A���������Ă���G�̂ݍX�V������ʂ�
			bool enableToUseDataFlag = m_enemies[enemyType][enemyCount] != nullptr && m_enemies[enemyType][enemyCount]->GetData()->oprationObjData->enableToHitFlag;
			if (enableToUseDataFlag)
			{
				//���Ԃ�0�b�ȉ��Ȃ�v���C���[�ɓ��������Ɣ��f����
				if (m_enemies[enemyType][enemyCount]->GetData()->timer <= 0)
				{
					m_player.Hit();
					//enemies[enemyType][enemyCount]->Dead();
				}
			}
		}
	}
	//�U��----------------------------------------------

	for (int enemyType = 0; enemyType < m_enemies.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < m_enemies[enemyType].size(); ++enemyCount)
		{
			//��������Ă���A���������Ă���G�̂ݍX�V������ʂ�
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
				//�����[�X�����S
				if (m_enemies[enemyType][enemyCount]->GetData()->oprationObjData->rockOnNum <= 0 &&
					m_cursor.Release())
				{
					m_enemies[enemyType][enemyCount]->Dead();
					//���b�V���p�[�e�B�N�����s����
					//m_meshParticleRender->AddMeshData(m_enemies[enemyType][enemyCount]->GetData()->meshParticleData[0]->meshParticleData);
				}
				m_enemies[enemyType][enemyCount]->Update();
			}


			//��̂ł��G�������Ă����炻���m�点��t���O���グ��
			if (enableToUseDataFlag && m_enemies[enemyType][enemyCount]->GetData()->oprationObjData->enableToHitFlag)
			{
				m_isEnemyNotMoveFlag = false;
			}
		}
	}

	//�Q�[���N���A
	bool gameClearFlag = !m_debugFlag && m_rail.IsEnd();
	if (gameClearFlag || KeyBoradInputManager::Instance()->InputTrigger(DIK_3))
	{
		m_sceneNum = 2;
	}
	//�Q�[���I�[�o�[
	if (!m_player.IsAlive() || KeyBoradInputManager::Instance()->InputTrigger(DIK_2))
	{
		m_sceneNum = 3;
	}


	m_rail.Update();
	m_stageArray[m_gameStageLevel]->playerPosZ = m_rail.GetPosition().z;
	m_stageArray[m_gameStageLevel]->Update();

	//�J�����ړ��ƃv���C���[���ړ�������
	m_player.Update();
	//���[���ړ�
	m_player.pos = m_rail.GetPosition();
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

	//�G���������Ă��Ȃ����Ԃ���莞�Ԃ𒴂�����Q�[�������Ԃ𑁂߂�
	if (KazMath::ConvertSecondToFlame(CHANGE_GMAE_FLAME_SPEED_MAX_TIME) <= m_notMoveTimer)
	{
		m_gameSpeed = 60;
	}
	else
	{
		m_gameSpeed = 1;
	}

	m_gameFlame += m_gameSpeed;


}

void InGame::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{
	m_player.Draw(arg_rasterize, arg_blasVec);

	PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, "Enemy");
	//�G�̕`�揈��----------------------------------------------------------------
	for (int enemyType = 0; enemyType < m_enemies.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < m_enemies[enemyType].size(); ++enemyCount)
		{
			//��������Ă���G�̂ݕ`�揈����ʂ�
			bool enableToUseDataFlag = m_enemies[enemyType][enemyCount] != nullptr &&
				m_enemies[enemyType][enemyCount]->GetData()->oprationObjData->initFlag &&
				!m_enemies[enemyType][enemyCount]->GetData()->outOfStageFlag;
			if (enableToUseDataFlag)
			{
				m_enemies[enemyType][enemyCount]->Draw(arg_rasterize, arg_blasVec);
			}
#ifdef _DEBUG
			if (enableToUseDataFlag && m_enemies[enemyType][enemyCount]->iOperationData.enableToHitFlag)
			{
				//�f�o�b�N���̂ݓ����蔻���`��
				//enemyHitBox[enemyType][enemyCount].Draw();
			}
#endif
		}
	}
	PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());

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

	arg_rasterize.ObjectRender(m_executeIndirect);

	DrawFunc::DrawModel(m_modelRender, KazMath::Transform3D(KazMath::Vec3<float>(), KazMath::Vec3<float>(6.0f, 6.0f, 6.0f), KazMath::Vec3<float>()));
	if (!KeyBoradInputManager::Instance()->InputState(DIK_SPACE))
	{
		arg_rasterize.ObjectRender(m_modelRender);
	}
	//m_stageArray[m_gameStageLevel]->Draw(arg_rasterize);

	ImGui::Begin("Game");
	ImGui::Checkbox("Debug", &m_debugFlag);
	ImGui::End();
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
