#pragma once
#include"../Interface/IEnemy.h"
#include"../Game/Effect/RocketEffect.h"

class NormalMisileEnemy:public IEnemy
{
public:
	NormalMisileEnemy();

	void Init(const EnemyGenerateData &GENERATE_DATA, bool DEMO_FLAG = false);
	void Finalize();
	void Update();
	void Draw();

private:
	int shotTimer;//ƒ~ƒTƒCƒ‹”­ŽË‚Ü‚Å‚ÌŽžŠÔ
	bool initShotFlag;
	FbxModelRender model;

	int flashTimer, disappearTimer;
	int circleFlashTimer;
	bool startFlag;
	CircleRender circleFlashR;
	CircleRender flashR;

	bool shotFlag;
};
