#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Interface/IEnemy.h"
#include"../Game/Effect/RocketEffect.h"
#include"../Game/Event/DirectionalMarker.h"

/// <summary>
/// チュートリアルで使用する説明用のブロック
/// </summary>
class TutorialBlock :public IEnemy
{
public:
	TutorialBlock();

	void Init(const EnemyGenerateData &GENERATE_DATA, bool DEMO_FLAG);
	void Finalize();
	void Update();
	void Draw();

private:
	int timer;
	KazMath::Vec3<float>larpScale;

	DirectionalMarker marker;
	RESOURCE_HANDLE redBlockResourceHandle, greenBlockResourceHandle;

	float baseScale;
	bool deadFlag;
};