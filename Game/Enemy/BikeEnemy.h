#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Interface/IEnemy.h"
#include"../Game/Effect/SparkEmitter.h"

class BikeEnemy:public IEnemy
{
public:
	BikeEnemy();

	void Init(const EnemyGenerateData &GENERATE_DATA, bool DEMO_FLAG);
	void Finalize();
	void Update();
	void Draw();

private:
	KazMath::Vec3<float>basePos;
	KazMath::Vec3<float>kockBackPos;
	KazMath::Vec3<float>sparkPos;

	int appearTimer;
	LineRender line;

	static const float DISAPPEAR_Z_POS;

	BoxPolygonRender misileR, misileR2;

	SparkEmitter emitt;
};

