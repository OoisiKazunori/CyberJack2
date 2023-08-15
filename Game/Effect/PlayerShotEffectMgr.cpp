#include "PlayerShotEffectMgr.h"

void PlayerShotEffectMgr::Init()
{
	for (auto& index : m_effects) {

		index.Init();

	}
}

void PlayerShotEffectMgr::Generate(const KazMath::Vec3<float>* arg_refPlayerPoint, const KazMath::Vec3<float>* arg_refEnemyPoint, shared_ptr<IEnemy> arg_refEnemy)
{

	for (auto& index : m_effects) {

		if (index.m_isActive) continue;

		index.Generate(arg_refPlayerPoint, arg_refEnemyPoint, arg_refEnemy);

		break;

	}

}

void PlayerShotEffectMgr::Update()
{
	for (auto& index : m_effects) {

		index.Update();

	}
}

void PlayerShotEffectMgr::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{
	for (auto& index : m_effects) {

		index.Draw(arg_rasterize, arg_blasVec);

	}
}
