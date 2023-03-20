#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Interface/IEnemy.h"
#include"../Game/Effect/RocketLightEffect.h"
#include"../Game/Effect/MisileSmokeEmitter.h"

class SplineMisileForBattleShip:public IEnemy
{
public:
	SplineMisileForBattleShip();

	void Init(const EnemyGenerateData &GENERATE_DATA, bool DEMO_FLAG = false);
	void Finalize();
	void Update();
	void Draw();

private:
	int maxTime;		//合計時間
	int pointTime;		//制御点間を移動する時間
	int nowTime;		//現在の時間

	//スプライン曲線----------------------------------------------
	std::vector<KazMath::Vec3<float>> points;
	int startIndex;		//どの制御点同士を移動しているか
	float timeRate;		//制御点間の時間
	KazMath::Vec3<float> position;	//現在地
	BoxPolygonRender splineBox;
	std::array<BoxPolygonRender, 3> pointsRender;
	//スプライン曲線----------------------------------------------

	bool hitFlag;

	RocketLightEffect rocketLight;
	//MisileSmokeEmitter smokeEmitter;
};

