#include "PlayerDamegePartice.h"

const float PlayerDamegePartice::SPEED = 0.8f;

PlayerDamegePartice::PlayerDamegePartice()
{
	line.data.colorData = { 255,255,255,255 };
}

void PlayerDamegePartice::Init(const KazMath::Vec3<float> &POS, int ANGLE)
{
	vel.x = cosf(KazMath::AngleToRadian(ANGLE)) * SPEED;
	vel.z = sinf(KazMath::AngleToRadian(ANGLE)) * SPEED;

	float lLength = 1.0f;
	line.data.startPos = POS;
	line.data.endPos = POS;

	line.data.startPos += { KazMath::Rand<float>(lLength,-lLength),KazMath::Rand<float>(lLength,-lLength),KazMath::Rand<float>(lLength,-lLength) };
	line.data.endPos += { KazMath::Rand<float>(lLength,-lLength),KazMath::Rand<float>(lLength,-lLength),KazMath::Rand<float>(lLength,-lLength) };
	line.data.colorData.color.a = 255;
}

void PlayerDamegePartice::Update()
{
	line.data.startPos += vel;
	line.data.endPos += vel;

	line.data.colorData.color.a -= 5;
	if (line.data.colorData.color.a <= 0)
	{
		line.data.colorData.color.a = 0;
	}

	KazMath::Larp(0.0f, &vel.x, 0.1f);
	KazMath::Larp(0.0f, &vel.z, 0.1f);
}

void PlayerDamegePartice::Draw()
{
	if (0 < line.data.colorData.color.a)
	{
		line.Draw();
	}
}
