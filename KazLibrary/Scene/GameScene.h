#pragma once
#include"../Game/Game.h"
#include"../Game/InGame.h"
#include"../KazLibrary/Scene/SceneBase.h"
#include"../Game/Helper/EnemyHelper.h"
#include"../Game/Stage/FirstStage.h"
#include"../Game/Stage/SecondStage.h"
#include"../Game/Stage/ThridStage.h"
#include"../Game/Stage/RezStage.h"
#include"../Game/Stage/BlockParticleStage.h"
#include"../Game/Stage/ChildOfEdenStage.h"

class GameScene :public SceneBase
{
public:
	GameScene();
	~GameScene();

	void Init();
	void PreInit();
	void Finalize();
	void Input();
	void Update();
	void Draw();

	int SceneChange();

private:
	std::unique_ptr<InGame> game;//ゲームに関するデータ

	std::array<std::array<ResponeData, KazEnemyHelper::ENEMY_NUM_MAX>, KazEnemyHelper::ENEMY_TYPE_MAX>m_responeData;

	std::array<std::shared_ptr<IStage>, KazEnemyHelper::STAGE_NUM_MAX> stages;
	std::array<std::array<KazEnemyHelper::ForceCameraData, 10>, KazEnemyHelper::STAGE_NUM_MAX>cameraMoveArray;
	std::array<KazMath::Color, KazEnemyHelper::STAGE_NUM_MAX> backGroundColorArray;

};