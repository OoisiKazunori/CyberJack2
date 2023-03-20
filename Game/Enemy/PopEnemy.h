#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Interface/IEnemy.h"

class PopEnemy:public IEnemy
{
public:
	PopEnemy();

	void Init(const EnemyGenerateData &GENERATE_DATA, bool DEMO_FLAG);
	void Finalize();
	void Update();
	void Draw();

private:
	KazMath::Vec3<float> basePos;
	KazMath::Vec3<float> bottomPos;
	KazMath::Vec3<float> vel;
	bool reverseFlag;
	bool oldReverseFlag;

	bool reverseXFlag;
	bool firstTimeToJumpFlag;
};
