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
	z = 50.0f;
	stages[layerLevel] = std::make_shared<ChildOfEdenStage>();
	backGroundColorArray[layerLevel] = KazMath::Color(0, 0, 0, 255);

	//2-----------------------------------------------------------------
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].layerLevel = layerLevel;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].flame = 0;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].moveID = 0;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].generateData.initPos = { 30.0f,10.0f,z };
	++respoineIndex;

	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].layerLevel = layerLevel;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].flame = 0;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].moveID = 1;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].generateData.initPos = { -30.0f,10.0f,z };
	++respoineIndex;

	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].layerLevel = layerLevel;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].flame = 0;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].moveID = 2;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].generateData.initPos = { 30.0f,50.0f,z };
	++respoineIndex;

	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].layerLevel = layerLevel;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].flame = 0;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].moveID = 3;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].generateData.initPos = { -30.0f,50.0f,z };
	++respoineIndex;

	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].layerLevel = layerLevel;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].flame = 0;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].moveID = 4;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].generateData.initPos = { -30.0f,60.0f,z };
	++respoineIndex;

	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].layerLevel = layerLevel;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].flame = 0;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].moveID = 5;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].generateData.initPos = { -30.0f,20.0f,z };
	++respoineIndex;

	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].layerLevel = layerLevel;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].flame = 0;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].moveID = 6;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].generateData.initPos = { -30.0f,30.0f,z };
	++respoineIndex;

	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].layerLevel = layerLevel;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].flame = 0;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].moveID = 7;
	m_responeData[ENEMY_TYPE_VIRUS][respoineIndex].generateData.initPos = { -30.0f,40.0f,z };
	++respoineIndex;

	//m_responeData[ENEMY_TYPE_BUTTERFLY][respoineIndex].layerLevel = layerLevel;
	//m_responeData[ENEMY_TYPE_BUTTERFLY][respoineIndex].flame = 0;
	//m_responeData[ENEMY_TYPE_BUTTERFLY][respoineIndex].generateData.initPos = { 90.0f,10.0f,z };
	//++respoineIndex;

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

void GameScene::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{
	game->Draw(arg_rasterize, arg_blasVec);
}

int GameScene::SceneChange()
{
	int num = game->SceneChange();

	switch (num)
	{
	case 1:
		break;
	case -3:
		skipTurtorialFlag = true;
		return -2;
		break;
	default:
		return num;
		break;
	}
	return SCENE_NONE;
}
