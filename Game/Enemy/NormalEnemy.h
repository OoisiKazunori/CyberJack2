#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Interface/IEnemy.h"
#include"../Game/Effect/RocketEffect.h"

/// <summary>
/// ƒTƒ“ƒvƒ‹—p‚Ì“G
/// </summary>
class NormalEnemy:public IEnemy
{
public:
	NormalEnemy();

	void Init(const EnemyGenerateData &GENERATE_DATA, bool DEMO_FLAG);
	void Finalize();
	void Update();
	void Draw();

private:
	float speed;
	float alpha;
};

