﻿#include "GameScene.h"
#include"../Input/KeyBoradInputManager.h"
#include"../Input/ControllerInputManager.h"
#include"../Imgui/MyImgui.h"
#include"../Loader/ObjResourceMgr.h"
#include"../Helper/ResourceFilePass.h"
#include"../Game/Debug/ParameterMgr.h"
#include"../Math/KazMath.h"

GameScene::GameScene()
{
	skipTurtorialFlag = false;
	PreInit();
	firstGenerateFlag = true;
}

GameScene::~GameScene()
{
}

void GameScene::Init()
{
	game->Init(skipTurtorialFlag);
	skipTurtorialFlag = false;
}

void GameScene::PreInit()
{
	int respoineIndex = 0;
	int layerLevel = 0;
	int time = 60;
	float z = 0.0f;


#pragma region 敵の配置レベル1
	z = 450.0f;
	stages[layerLevel] = std::make_shared<ChildOfEdenStage>();
	backGroundColorArray[layerLevel] = KazMath::Color(0, 0, 0, 255);

	//2-----------------------------------------------------------------
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].layerLevel = layerLevel;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].flame = time * 10;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].generateData.initPos = { 50.0f,10.0f,z };
	++respoineIndex;

	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].layerLevel = layerLevel;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].flame = time * 10;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].generateData.initPos = { 90.0f,10.0f,z };
	++respoineIndex;
	//2-----------------------------------------------------------------

#pragma endregion
	++layerLevel;


	game = std::make_unique<InGame>(m_responeData, stages, backGroundColorArray, cameraMoveArray);
}

void GameScene::Finalize()
{
	game.reset();
	for (int i = 0; i < stages.size(); ++i)
	{
		stages[i].reset();
	}
}

void GameScene::Input()
{
	game->Input();
}

void GameScene::Update()
{
	game->Update();
}

void GameScene::Draw()
{
	game->Draw();
}

int GameScene::SceneChange()
{
	int lNum = game->SceneChange();
	if (lNum == 0)
	{
		return 0;
	}
	else if (lNum == -3)
	{
		skipTurtorialFlag = true;
		return -2;
	}
	else if (ControllerInputManager::Instance()->InputTrigger(XINPUT_GAMEPAD_BACK))
	{
		return 0;
	}
	else
	{
		return SCENE_NONE;
	}
	return SCENE_NONE;
}
