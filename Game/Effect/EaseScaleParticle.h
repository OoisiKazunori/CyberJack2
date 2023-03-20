#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../KazLibrary/Easing/easing.h"

class EaseScaleParticle
{
public:
	EaseScaleParticle(int GRAPH_HANDLE);

	void Init(
		const KazMath::Vec2<float> &POS,
		const KazMath::Vec2<float> &BASE_SCALE,
		const KazMath::Vec2<float> &EASE_SCALE,
		const KazMath::Vec2<int> &TIMER,
		bool ROTA_FLAG
	);
	void Update();
	void Draw();

	bool IsAlive();
private:
	KazMath::Vec2<float> baseScale, easeScale;
	Sprite2DRender render;
	KazMath::Vec2<float> easeMaxTimer;
	static const float MAX_RATE;
	KazMath::Vec2<float> rate;
	bool initFlag;
	bool rotaFlag;
};

