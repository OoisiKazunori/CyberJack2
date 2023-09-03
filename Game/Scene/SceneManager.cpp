#include "SceneManager.h"
#include"../../KazLibrary/Helper/KazHelper.h"
#include"../KazLibrary/Sound/SoundManager.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"
#include"../KazLibrary/Fps/FPSManager.h"
#include"GameScene.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../Game/Effect/ShakeMgr.h"
#include"../Scene/ModelToolScene.h"
#include"../Game/Effect/ShockWave.h"
#include"../../Game/Effect/StopMgr.h"
#include <Input/ControllerInputManager.h>
#include"../KazLibrary/Sound/SoundManager.h"

SceneManager::SceneManager() :gameFirstInitFlag(false)
{
	SoundManager::Instance()->SettingSoundManager();

	//デモ用のゲームシーンを設定。
	scene.emplace_back(std::make_unique<DemoScene>());

	//シーン遷移を設定
	change = std::make_unique<ChangeScene::SceneChange>();

	//シーン番号、遷移に関するパラメーターを設定。
	nowScene = 0;
	nextScene = 0;
	itisInArrayFlag = true;
	endGameFlag = false;
	initGameFlag = false;

	//レイトレーシング周りを設定
	Raytracing::HitGroupMgr::Instance()->Setting();
	m_pipelineShaders.push_back({ "Resource/ShaderFiles/RayTracing/RaytracingShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS", L"checkHitRayMS"}, {L"mainCHS", L"mainAnyHit"} });
	int payloadSize = sizeof(float) * 7;
	m_rayPipeline = std::make_unique<Raytracing::RayPipeline>(m_pipelineShaders, Raytracing::HitGroupMgr::DEF, 6, 5, 4, payloadSize, static_cast<int>(sizeof(KazMath::Vec2<float>)), 6);


	//ヒットストップのタイマーを初期化。
	StopMgr::Instance()->Init();

	ShockWave::Instance()->Setting();
}

SceneManager::~SceneManager()
{
}

void SceneManager::Update()
{
	DescriptorHeapMgr::Instance()->SetDescriptorHeap();

	if (StopMgr::Instance()->IsHitStop()) {
		StopMgr::Instance()->Update();
		m_blasVector.Update();
		return;
	}

	//シェイク量を更新。
	ShakeMgr::Instance()->Update();

	//シーン遷移の開始
	if (nextScene != nowScene)
	{
		change->Start();
	}

	if (!gameFirstInitFlag)
	{
		scene[nowScene]->Init();
		gameFirstInitFlag = true;
		initGameFlag = false;
	}

	const int RESTART_NUM = -2;

	//画面が完全に隠れてから1F分ずらす
	if (initGameFlag)
	{
		if (KazHelper::IsitInAnArray(nowScene, scene.size()))
		{
			scene[nowScene]->Init();
			itisInArrayFlag = true;
		}
		else
		{
			itisInArrayFlag = false;
		}
		initGameFlag = false;
	}

	//ゲーム画面が隠された判定
	if (change->AllHiden())
	{
		scene[nowScene]->Finalize();

		if (nextScene != RESTART_NUM)
		{
			nowScene = nextScene;
		}
		else if (nextScene == RESTART_NUM)
		{
			nextScene = nowScene;
		}

		if (!scene[nextScene]->firstGenerateFlag)
		{
			scene[nextScene]->PreInit();
		}
		scene[nextScene]->firstGenerateFlag = false;
		initGameFlag = true;
	}

	//更新処理
	if (itisInArrayFlag && !initGameFlag)
	{
		scene[nowScene]->Input();
		scene[nowScene]->Update();

		int sceneNum = scene[nowScene]->SceneChange();
		if (sceneNum != SCENE_NONE)
		{
			nextScene = sceneNum;
		}

		if (scene[nowScene]->endGameFlag)
		{
			endGameFlag = true;
		}
	}

	change->Update();

	m_blasVector.Update();

	//fpsを制限(今回は60fps)
	FpsManager::RegulateFps(60);


	//データを転送。一旦ここで。
	GBufferMgr::Instance()->m_lightBuffer.bufferWrapper->TransData(&GBufferMgr::Instance()->m_lightConstData, sizeof(GBufferMgr::LightConstData));

}

void SceneManager::Draw()
{
	change->Draw(m_rasterize);


	if (itisInArrayFlag)
	{
		scene[nowScene]->Draw(m_rasterize, m_blasVector);
	}

	m_rasterize.Sort();
	m_rasterize.Render();

	//Tlasを構築 or 再構築する。
	m_tlas.Build(m_blasVector);
	//レイトレ用のデータを構築。
	m_rayPipeline->BuildShaderTable(m_blasVector);
	if (m_blasVector.GetBlasRefCount() != 0)
	{
		m_rayPipeline->TraceRay(m_tlas);
	}

}
