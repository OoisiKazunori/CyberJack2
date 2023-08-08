#include "InGame.h"
#include"../KazLibrary/Buffer/UavViewHandleMgr.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../KazLibrary/Input/ControllerInputManager.h"

InGame::InGame(const std::array<std::array<ResponeData, KazEnemyHelper::ENEMY_NUM_MAX>, KazEnemyHelper::ENEMY_TYPE_MAX>& arg_responeData, const std::array<std::shared_ptr<IStage>, KazEnemyHelper::STAGE_NUM_MAX>& arg_stageArray, const std::array<KazMath::Color, KazEnemyHelper::STAGE_NUM_MAX>& BACKGROUND_COLOR, const std::array<std::array<KazEnemyHelper::ForceCameraData, 10>, KazEnemyHelper::STAGE_NUM_MAX>& CAMERA_ARRAY) :
	m_stageArray(arg_stageArray),m_responeData(arg_responeData)
{
	KazEnemyHelper::GenerateEnemy(m_enemies, m_responeData, enemiesHandle, m_enemyHitBoxArray);

	m_debugFlag = false;

	Raytracing::HitGroupMgr::Instance()->Setting();
	m_pipelineShaders.push_back({ "Resource/ShaderFiles/RayTracing/RaytracingShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	int payloadSize = sizeof(float) * 4;
	m_rayPipeline = std::make_unique<Raytracing::RayPipeline>(m_pipelineShaders, Raytracing::HitGroupMgr::DEF, 5, 3, 2, payloadSize, static_cast<int>(sizeof(KazMath::Vec2<float>)), 6);



	//ボリュームテクスチャを生成。
	m_volumeFogTextureBuffer = KazBufferHelper::SetUAV3DTexBuffer(256, 256, 256, DXGI_FORMAT_R8G8B8A8_UNORM);
	m_volumeFogTextureBuffer.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
	DescriptorHeapMgr::Instance()->CreateBufferView(
		m_volumeFogTextureBuffer.bufferWrapper->GetViewHandle(),
		KazBufferHelper::SetUnorderedAccess3DTextureView(sizeof(DirectX::XMFLOAT4), 256 * 256 * 256),
		m_volumeFogTextureBuffer.bufferWrapper->GetBuffer().Get()
	);
	//ボリュームノイズパラメーター
	m_noiseParamData = KazBufferHelper::SetConstBufferData(sizeof(NoiseParam));
	//ボリュームノイズのパラメーターを設定
	m_noiseParam.m_timer = 0.0f;
	m_noiseParam.m_windSpeed = 10.00f;
	m_noiseParam.m_windStrength = 1.0f;
	m_noiseParam.m_threshold = 0.42f;
	m_noiseParam.m_skydormScale = 356.0f;
	m_noiseParam.m_octaves = 4;
	m_noiseParam.m_persistence = 0.5f;
	m_noiseParam.m_lacunarity = 2.5f;
	m_noiseParamData.bufferWrapper->TransData(&m_noiseParam, sizeof(NoiseParam));
	//ボリュームノイズ書き込み
	{
		std::vector<KazBufferHelper::BufferData>extraBuffer =
		{
			 m_volumeFogTextureBuffer,
			 m_noiseParamData
		};
		extraBuffer[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		extraBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_TEX;
		extraBuffer[1].rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		extraBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_DATA;
		m_volumeNoiseShader.Generate(ShaderOptionData(KazFilePathName::RelativeShaderPath + "Raytracing/" + "Write3DNoise.hlsl", "CSmain", "cs_6_4", SHADER_TYPE_COMPUTE), extraBuffer);
		m_rayPipeline->SetVolumeFogTexture(&m_volumeFogTextureBuffer);
	}

	//レイマーチングのパラメーターを設定。
	m_raymarchingParam.m_pos = KazMath::Vec3<float>();
	m_raymarchingParam.m_color = KazMath::Vec3<float>(1.0f, 1.0f, 1.0f);
	m_raymarchingParam.m_wrapCount = 20.0f;
	m_raymarchingParam.m_gridSize = 4.0f;
	m_raymarchingParam.m_wrapCount = 30.0f;
	m_raymarchingParam.m_density = 0.65f;
	//m_raymarchingParam.m_density = 1.0f;
	m_raymarchingParam.m_sampleLength = 30.0f;
	m_raymarchingParam.m_isSimpleFog = 0;
	m_raymarchingParamData = KazBufferHelper::SetConstBufferData(sizeof(RaymarchingParam));
	m_raymarchingParamData.bufferWrapper->TransData(&m_raymarchingParam, sizeof(RaymarchingParam));

	//レイマーチングのパラメーター用定数バッファをセット。
	m_rayPipeline->SetRaymarchingConstData(&m_raymarchingParamData);
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
}

void InGame::Finalize()
{
	m_player.Finalize();
}

void InGame::Input()
{
	KeyBoradInputManager* input = KeyBoradInputManager::Instance();
	if (input->InputTrigger(DIK_ESCAPE))
	{
		Init(false);
	}
	m_player.Input();
}

void InGame::Update()
{

#pragma region 生成処理
	//敵を追加で初期化する処理----------------------------------------------------------------
	KazEnemyHelper::AddEnemy(m_enemies, m_responeData, addEnemiesHandle, m_gameFlame, m_gameStageLevel);
	//敵を追加で初期化する処理----------------------------------------------------------------


	//敵をどのタイミングで初期化する処理----------------------------------------------------------------
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
				m_enemies[enemyType][enemyCount]->Init(m_responeData[enemyType][enemyCount].generateData, false);

				if (m_enemies[enemyType][enemyCount]->GetData()->meshParticleFlag)
				{
					continue;
				}
			}
		}
	}
	//敵をどのタイミングで初期化する処理----------------------------------------------------------------
#pragma endregion


#pragma region 敵ロックオン
	for (int enemyType = 0; enemyType < m_enemies.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < m_enemies[enemyType].size(); ++enemyCount)
		{
			//生成されている、初期化している敵のみ更新処理を通す
			bool enableToUseDataFlag = m_enemies[enemyType][enemyCount] != nullptr && m_enemies[enemyType][enemyCount]->GetData()->oprationObjData->initFlag;
			if (enableToUseDataFlag)
			{
				//敵のデータのポインタを代入
				EnemyData* enemyData = m_enemies[enemyType][enemyCount]->GetData().get();

				//ロックオン判定
				bool enableToLockOnNumFlag = m_cursor.LockOn();
				bool enableToLockOnEnemyFlag = m_enemies[enemyType][enemyCount]->IsAlive() && !m_enemies[enemyType][enemyCount]->LockedOrNot();
				bool hitFlag = CollisionManager::Instance()->CheckRayAndSphere(m_cursor.hitBox, enemyData->hitBox);
				if (hitFlag &&
					enableToLockOnNumFlag &&
					enableToLockOnEnemyFlag &&
					!m_cursor.releaseFlag)
				{

					//カーソルのカウント数を増やす
					m_cursor.Hit(enemyData->hitBox.center);
					//敵が当たった情報を書く
					m_enemies[enemyType][enemyCount]->Hit();

					//stringLog.WriteLog(enemies[enemyType][enemyCount]->GetData()->oprationObjData->name, LOG_FONT_SIZE);

					////線演出をかける際にどの配列を使用するか決める
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
			//生成されている、初期化している敵のみ更新処理を通す
			bool enableToUseDataFlag = m_enemies[enemyType][enemyCount] != nullptr && m_enemies[enemyType][enemyCount]->GetData()->oprationObjData->initFlag;
			if (enableToUseDataFlag)
			{
				//敵のデータのポインタを代入
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
	//		//生成されている、初期化している敵のみ更新処理を通す
	//		bool enableToUseDataFlag = enemies[enemyType][enemyCount] != nullptr && enemies[enemyType][enemyCount]->GetData()->oprationObjData->initFlag;
	//		if (enableToUseDataFlag)
	//		{
	//			//敵のデータのポインタを代入
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



#pragma region 線のロックオン解放

	//線がたどり着いたら敵を死亡させる
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

	//		//演出を合わせてダメージと死亡をやる
	//		if (lineLevel[lineIndex].lineReachObjFlag && !lineLevel[lineIndex].alreadyReachedFlag && enemies[enemyTypeIndex][enemyIndex]->IsAlive())
	//		{
	//			SoundManager::Instance()->PlaySoundMem(damageSoundHandle, 1);
	//			lineLevel[lineIndex].alreadyReachedFlag = true;
	//		}
	//		else if (lineLevel[lineIndex].lineReachObjFlag && !enemies[enemyTypeIndex][enemyIndex]->IsAlive() && !lineEffectArrayData[i].hitFlag)
	//		{
	//			enemies[enemyTypeIndex][enemyIndex]->Dead();
	//			lineEffectArrayData[i].hitFlag = true;

	//			//ヒット時の円演出
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

	////線演出が終わった際にリンクのデータをリセットする
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

	//攻撃----------------------------------------------
	for (int enemyType = 0; enemyType < m_enemies.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < m_enemies[enemyType].size(); ++enemyCount)
		{
			//生成されている、初期化している敵のみ更新処理を通す
			bool enableToUseDataFlag = m_enemies[enemyType][enemyCount] != nullptr && m_enemies[enemyType][enemyCount]->GetData()->oprationObjData->enableToHitFlag;
			if (enableToUseDataFlag)
			{
				//時間が0秒以下ならプレイヤーに当たったと判断する
				if (m_enemies[enemyType][enemyCount]->GetData()->timer <= 0)
				{
					m_player.Hit();
					//enemies[enemyType][enemyCount]->Dead();
				}
			}
		}
	}
	//攻撃----------------------------------------------

	for (int enemyType = 0; enemyType < m_enemies.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < m_enemies[enemyType].size(); ++enemyCount)
		{
			//生成されている、初期化している敵のみ更新処理を通す
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
				m_enemies[enemyType][enemyCount]->Update();
			}

			//一体でも敵が動いていたらそれを知らせるフラグを上げる
			if (enableToUseDataFlag && m_enemies[enemyType][enemyCount]->GetData()->oprationObjData->enableToHitFlag)
			{
				m_isEnemyNotMoveFlag = false;
			}
		}
	}


	m_player.Update();
	m_rail.Update();
	m_stageArray[m_gameStageLevel]->Update();

	//レール移動
	m_player.pos = m_rail.GetPosition();
	//カメラ移動とプレイヤーを移動させる
	m_player.Update();
	m_cursor.Update();
	m_camera.Update(m_cursor.GetValue(), &m_player.pos, false);

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

	//敵が何もしていない時間が一定時間を超えたらゲーム内時間を早める
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

void InGame::Draw(DrawingByRasterize& arg_rasterize)
{
	m_player.Draw(arg_rasterize);
	//m_cursor.Draw();

#ifdef _DEBUG
	if (m_debugFlag)
	{
		m_rail.DebugDraw(arg_rasterize);
	}
#endif

	PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, "Enemy");
	//敵の描画処理----------------------------------------------------------------
	for (int enemyType = 0; enemyType < m_enemies.size(); ++enemyType)
	{
		for (int enemyCount = 0; enemyCount < m_enemies[enemyType].size(); ++enemyCount)
		{
			//生成されている敵のみ描画処理を通す
			bool enableToUseDataFlag = m_enemies[enemyType][enemyCount] != nullptr &&
				m_enemies[enemyType][enemyCount]->GetData()->oprationObjData->initFlag &&
				!m_enemies[enemyType][enemyCount]->GetData()->outOfStageFlag;
			if (enableToUseDataFlag)
			{
				m_enemies[enemyType][enemyCount]->Draw(arg_rasterize);
			}
#ifdef _DEBUG
			if (enableToUseDataFlag && m_enemies[enemyType][enemyCount]->iOperationData.enableToHitFlag)
			{
				//デバック時のみ当たり判定を描画
				//enemyHitBox[enemyType][enemyCount].Draw();
			}
#endif
		}
	}
	PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());

	m_stageArray[m_gameStageLevel]->Draw(arg_rasterize);

	ImGui::Begin("Game");
	ImGui::Checkbox("Debug", &m_debugFlag);
	ImGui::End();
}

int InGame::SceneChange()
{
	return -1;
}
