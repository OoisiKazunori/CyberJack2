#pragma once
#include"../KazLibrary/Render/KazRender.h"

class PlayerDamegePartice
{
public:
	PlayerDamegePartice();
	void Init(const KazMath::Vec3<float> &POS, int ANGLE);
	void Update();
	void Draw();

private:
	LineRender line;
	KazMath::Vec3<float>vel;
	static const float SPEED;
};

