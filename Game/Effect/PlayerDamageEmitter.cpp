#include "PlayerDamageEmitter.h"

PlayerDamageEmitter::PlayerDamageEmitter()
{
}

void PlayerDamageEmitter::Init(const KazMath::Vec3<float> &POS)
{
	pos = POS;

	for (int i = 0; i < particle.size(); ++i)
	{
		particle[i].Init(pos, KazMath::Rand(360, 0));
	}
}

void PlayerDamageEmitter::Update()
{
	for (int i = 0; i < particle.size(); ++i)
	{
		particle[i].Update();
	}
}

void PlayerDamageEmitter::Draw()
{
	for (int i = 0; i < particle.size(); ++i)
	{
		particle[i].Draw();
	}
}
