#include "SceneManager.h"
#include"../Helper/KazHelper.h"
#include"../Sound/SoundManager.h"
#include"../Sound/SoundManager.h"
#include"../Helper/ResourceFilePass.h"
#include"../Buffer/DescriptorHeapMgr.h"
#include"../Fps/FPSManager.h"
#include"../Scene/PortalScene.h"
#include"../Scene/EnemyDebugScene.h"
#include"../Scene/DebugStageScene.h"
#include"../Scene/DebugMeshParticle.h"
#include"../Scene/RayTracingScene.h"
#include"../Scene/RenderScene.h"
#include"../Scene/ModelToolScene.h"

SceneManager::SceneManager() :gameFirstInitFlag(false)
{
	scene.emplace_back(std::make_unique<ModelToolScene>());
	
	nowScene = 0;
	nextScene = 0;
	itisInArrayFlag = true;
	endGameFlag = false;
	initGameFlag = false;

	change = std::make_unique<ChangeScene::SceneChange>();
}

SceneManager::~SceneManager()
{
	SoundManager::Instance()->Finalize();
}

void SceneManager::Update()
{
	DescriptorHeapMgr::Instance()->SetDescriptorHeap();

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

	// fpsを制限(今回は60fps)
	FpsManager::RegulateFps(60);
}

void SceneManager::Draw() {

	if (itisInArrayFlag)
	{
		scene[nowScene]->Draw();
	}

	//change.Draw();
	//RenderTargetStatus::Instance()->SwapResourceBarrier();
}
