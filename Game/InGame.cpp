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
#include"../Game/Effect/TimeZone.h"
#include"../Game/UI/OptionUI.h"
#include"../KazLibrary/Easing/easing.h"

InGame::InGame(const std::array<std::array<ResponeData, KazEnemyHelper::ENEMY_NUM_MAX>, KazEnemyHelper::ENEMY_TYPE_MAX>& arg_responeData, const std::array<std::shared_ptr<IStage>, KazEnemyHelper::STAGE_NUM_MAX>& arg_stageArray, const std::array<KazMath::Color, KazEnemyHelper::STAGE_NUM_MAX>& BACKGROUND_COLOR, const std::array<std::array<KazEnemyHelper::ForceCameraData, 10>, KazEnemyHelper::STAGE_NUM_MAX>& CAMERA_ARRAY) :
	m_stageArray(arg_stageArray), m_responeData(arg_responeData), m_sceneNum(-1)
{
	KazEnemyHelper::GenerateEnemy(m_enemies, m_responeData, enemiesHandle, m_enemyHitBoxArray);

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
	m_gameSpeed = 1;
	m_noEnemyTimer = 0;
	m_isNoEnemy = false;
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
	m_guideTimer = 0;
	m_guideAlphaTimer = 0.0f;
	m_appearGuideFlag = true;
	m_isStartFlag = false;
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

	KazMath::Vec2<float>mouseVel(input->GetMouseVel().x, input->GetMouseVel().y);

	const int DEAD_ZONE = 3000;
	bool doneFlag = cInput->InputState(XINPUT_GAMEPAD_A) || input->MouseInputState(MOUSE_INPUT_LEFT);
	bool releaseFlag = cInput->InputRelease(XINPUT_GAMEPAD_A) || input->MouseInputRelease(MOUSE_INPUT_LEFT);

	KazMath::Vec2<float>dir = mouseVel - m_prevMouseVel;
	bool upFlag = cInput->InputStickState(LEFT_STICK, UP_SIDE, DEAD_ZONE) || std::signbit(dir.y);
	bool downFlag = cInput->InputStickState(LEFT_STICK, DOWN_SIDE, DEAD_ZONE) || !std::signbit(dir.y);
	bool leftFlag = cInput->InputStickState(LEFT_STICK, LEFT_SIDE, DEAD_ZONE) || std::signbit(dir.x);
	bool rightFlag = cInput->InputStickState(LEFT_STICK, RIGHT_SIDE, DEAD_ZONE) || !std::signbit(dir.x);

	if (input->InputTrigger(DIK_I) || cInput->InputTrigger(XINPUT_GAMEPAD_START))
	{
		m_appearGuideFlag = !m_appearGuideFlag;
	}

	if (input->InputTrigger(DIK_SPACE))
	{
		m_guideTimer = 60 * 3;
	}

	if (input->InputTrigger(DIK_SPACE) || cInput->InputTrigger(XINPUT_GAMEPAD_Y))
	{
		TimeZone::Instance()->m_timeZone = !TimeZone::Instance()->m_timeZone;
		OptionUI::Instance()->m_optionDetails[OptionUI::TIMEZONE].m_selectID = !OptionUI::Instance()->m_optionDetails[OptionUI::TIMEZONE].m_selectID;
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

	//敵の当たり判定
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
					m_enemies[enemyType][enemyCount]->GetCanLockOn())
				{

					SoundManager::Instance()->SoundPlayerWave(m_lockonSE, 0);

					m_cursor.Hit(enemyData->hitBox.center);

					m_enemies[enemyType][enemyCount]->Hit();
				}
			}
		}
	}


	m_isNoEnemy = true;
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
				m_isNoEnemy = false;
			}
		}
	}

	m_rail.Update();
	m_stageArray[m_gameStageLevel]->playerPos = m_rail.GetPosition();

	m_player.Update();
	m_player.pos = {0,20,0};
	m_cursor.Update();
	m_camera.Update(m_cursor.GetValue(), &m_player.pos, m_player.m_transform.rotation, false);


	if (m_isNoEnemy)
	{
		++m_noEnemyTimer;
	}
	else
	{
		m_noEnemyTimer = 0;
	}

	const int ENEMY_RESPAWN_TIMER = 180;
	if (ENEMY_RESPAWN_TIMER == m_noEnemyTimer)
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

	CameraMgr::Instance()->Camera(m_camera.GetEyePos(), m_camera.GetTargetPos(), { 0,1,0 });

	PlayerShotEffectMgr::Instance()->Update(&m_camera.GetEyePos(), m_stageArray[m_gameStageLevel]->hitFlag, &m_lightIndex);


	//デバッグで無理やり衝撃波を出す。
	if (KeyBoradInputManager::Instance()->InputTrigger(DIK_RETURN) || ControllerInputManager::Instance()->InputTrigger(XINPUT_GAMEPAD_B)) {

		//ランダムで敵を選択して衝撃波を出す。
		int randomEnemy = KazMath::Rand(0, 7);

		ShakeMgr::Instance()->m_shakeAmount = 0.4f;
		SeaEffect::Instance()->m_isSeaEffect = true;
		ShockWave::Instance()->m_shockWave[randomEnemy].m_isActive = true;
		ShockWave::Instance()->m_shockWave[randomEnemy].m_power = 1.0f;
		ShockWave::Instance()->m_shockWave[randomEnemy].m_radius = 0.0f;
		m_enemies[ENEMY_TYPE_VIRUS][randomEnemy]->InitShockWaveTimer();
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
		if (!m_isStartFlag && alpha <= 0.0f)
		{
			OptionUI::Instance()->m_optionDetails[OptionUI::RAYTRACING].m_selectID = false;
			m_isStartFlag = true;
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

	m_cursor.Draw(arg_rasterize);


	{
		KazMath::Color color = KazMath::Color(255, 255, 255, 255);
		KazMath::Transform2D transform;
		transform.scale = KazMath::Vec2<float>(static_cast<float>(m_guideTex.bufferWrapper->GetBuffer()->GetDesc().Width), static_cast<float>(m_guideTex.bufferWrapper->GetBuffer()->GetDesc().Height));

		float posY = 0.0f;
		if (m_appearGuideFlag)
		{
			Rate(&m_appearGuideRate, 0.08f, 1.0f);
			posY = transform.scale.y + EasingMaker(Out, Exp, m_appearGuideRate) * -transform.scale.y;
			m_disappearGuideRate = 0.0f;
		}
		else
		{
			Rate(&m_disappearGuideRate, 0.08f, 1.0f);
			posY = EasingMaker(Out, Exp, m_disappearGuideRate) * transform.scale.y;
			m_appearGuideRate = 0.0f;
		}

		transform.pos = KazMath::Vec2<float>(1280.0f - transform.scale.x / 2.0f, 720.0f - transform.scale.y / 2.0f + posY);
		DrawFunc::DrawTextureIn2D(m_guideUI, transform, m_guideTex, color);
		arg_rasterize.ObjectRender(m_guideUI);
	}

	m_stageArray[m_gameStageLevel]->Draw(arg_rasterize, arg_blasVec);

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
