#pragma
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Render/KazRender.h"

class HitEnemyEffect
{
public:
	HitEnemyEffect();

	void Update();
	void Draw();

	void Start(const KazMath::Vec3<float> &POS);
	bool IsAlive();
private:
	enum
	{
		BIG_CIRCLE,
		SMALL_CIRCLE
	};
	bool startFlag;
	KazMath::Vec3<float>pos;
	std::array<CircleRender, 2>circleR;
	std::array<int, 2>timer;
	int maxTimer;
	std::array<float, 2>baseRadius;
	float maxRadius;
	std::array<float, 2>addRadius;


};
