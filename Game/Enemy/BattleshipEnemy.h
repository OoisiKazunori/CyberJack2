#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Interface/IEnemy.h"

class BattleshipEnemy:public IEnemy
{
public:
	BattleshipEnemy();

	void Init(const EnemyGenerateData &GENERATE_DATA, bool DEMO_FLAG);
	void Finalize();
	void Update();
	void Draw();

private:
	KazMath::Vec3<float>basePos;
	KazMath::Vec3<float>adjHitBoxPos;
	KazMath::Vec3<float>hitBoxPos;
	int appearTimer;
	int disappearTime;
	float vel;
	int misileShotIndex;
	std::array<BoxPolygonRender, 8>misileR;
	FbxModelRenderPtr model;

	KazMath::Vec3<float>topPos;
	KazMath::Vec3<float>topRota;
	FbxModelRenderPtr topModel;
	bool isShotFlag;


	KazMath::Vec3<float>lightDir;
};

