#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Effect/PlayerDamegePartice.h"

class PlayerDamageEmitter
{
public:
	PlayerDamageEmitter();
	void Init(const KazMath::Vec3<float> &POS);
	void Update();
	void Draw();

private:
	KazMath::Vec3<float>pos;
	std::array<PlayerDamegePartice, 50>particle;
};
