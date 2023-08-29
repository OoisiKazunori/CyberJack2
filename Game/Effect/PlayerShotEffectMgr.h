#pragma once
#include "PlayerShotEffect.h"
#include "../KazLibrary/Helper/ISinglton.h"
#include <array>
#include <memory>
#include <Math/KazMath.h>
#include "../Interface/IEnemy.h"

class PlayerShotEffectMgr : public ISingleton<PlayerShotEffectMgr> {

private:

	static const int EFFECT_NUM = 10;
	std::array<PlayerShotEffect, EFFECT_NUM> m_effects;
	std::vector<std::shared_ptr<IEnemy>> m_enemyStack;
	std::vector<std::array<std::shared_ptr<IEnemy>, 7>> m_otherEnemyStack;

	int m_effectDelay;
	const int EFFECT_DELAY = 10;

public:

	PlayerShotEffectMgr();
	void Init();
	void Generate(shared_ptr<IEnemy> arg_refEnemy, std::array<std::shared_ptr<IEnemy>, 7> arg_refOtherEnemy);
	void Update(const KazMath::Vec3<float>* arg_refPlayerPoint, std::array<bool, 16>& arg_hitArray, int* arg_hitNum);
	void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec);

	int GetAliveCount()
	{
		int count = 0;
		for (auto& obj : m_effects)
		{
			if (obj.m_isActive)
			{
				++count;
			}
		}
		return count;
	}
};