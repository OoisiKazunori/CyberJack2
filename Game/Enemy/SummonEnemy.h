#pragma once
#include"../Game/Interface/IEnemy.h"

class SummonEnemy :public IEnemy
{
public:
	SummonEnemy();

	void Init(const EnemyGenerateData &GENERATE_DATA, bool DEMO_FLAG = false) override;
	void Finalize()override;
	void Update()override;
	void Draw()override;

private:
	KazMath::Vec3<float>basePos;
	int appearTimer, summonTime, disappearTime;

	enum Status
	{
		APPEAR,
		SUMMON,
		DISAPPEAR
	};

	Status status;

	std::array<BoxPolygonRender, 2> summonR;
	std::array<KazMath::Vec3<float>, 2> summonPos;
	float rotaMaxAngle;
};
