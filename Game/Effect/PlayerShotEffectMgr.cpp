#include "PlayerShotEffectMgr.h"

PlayerShotEffectMgr::PlayerShotEffectMgr()
{
}

void PlayerShotEffectMgr::Init()
{
	for (auto& index : m_effects) {

		index.Init();

	}
	m_effectDelay = 0;
}

void PlayerShotEffectMgr::Generate(shared_ptr<IEnemy> arg_refEnemy, std::array<std::shared_ptr<IEnemy>, 7> arg_refOtherEnemy)
{

	m_enemyStack.emplace_back(arg_refEnemy);
	m_otherEnemyStack.emplace_back(arg_refOtherEnemy);

}

void PlayerShotEffectMgr::Update(const KazMath::Vec3<float>* arg_refPlayerPoint, std::array<bool, 16>& arg_hitArray, int* arg_hitNum)
{

	++m_effectDelay;
	if (EFFECT_DELAY <= m_effectDelay && 0 < static_cast<int>(m_enemyStack.size())) {

		for (auto& index : m_effects) {

			if (index.m_isActive) continue;

			index.Generate(arg_refPlayerPoint, m_enemyStack.back(), m_otherEnemyStack.back());
			m_enemyStack.pop_back();
			m_otherEnemyStack.pop_back();

			break;

		}

		m_effectDelay = 0;

	}

	for (auto& index : m_effects) {

		if (!index.m_isActive) continue;

		index.Update(arg_hitArray, arg_hitNum);

	}
}

void PlayerShotEffectMgr::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{
	for (auto& index : m_effects) {

		//if (!index.m_isActive) continue;

		index.Draw(arg_rasterize, arg_blasVec);

	}
}
