#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Render/KazRender.h"

class LockOnBoxEffect
{
public:
	LockOnBoxEffect();

	void Start(KazMath::Vec3<float> *POS);
	void Update();
	void Draw();

	bool IsAlive();
private:
	Box2DRender box;
	KazMath::Vec2<float>lockOnPos;
	KazMath::Vec3<float>*targetPos;
	float boxAngle;
	int boxDisappearTimer;
	bool aliveFlag;
	static const int BOX_DISAPPEAR_MAX_TIMER;
	static const float BOX_ROTA_SPEED;

	std::array<KazMath::Vec2<float>, 4>boxSize;
	enum
	{
		LEFT_UP,
		LEFT_DOWN,
		RIGHT_UP,
		RIGHT_DOWN,
	};


	void CaluBox(const KazMath::Vec2<float> &START_POS, const KazMath::Vec2<float> &END_POS, KazMath::Vec2<float> *NOW_POS, int TIMER, int MAX_TIMER)
	{
		KazMath::Vec2<float>lCpos = END_POS;
		KazMath::Vec2<float>distance = END_POS - START_POS;
		KazMath::Vec2<float>lPos = START_POS + distance * (static_cast<float>(TIMER) / static_cast<float>(MAX_TIMER));

		*NOW_POS = KazMath::CaluAngle(lPos, static_cast<float>(boxAngle), lCpos);
	}
};

