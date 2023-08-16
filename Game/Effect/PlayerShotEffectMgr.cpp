#include "PlayerShotEffectMgr.h"

PlayerShotEffectMgr::PlayerShotEffectMgr()
{
}

void PlayerShotEffectMgr::Init()
{
	for (auto& index : m_effects) {

		index.Init();

	}
}

void PlayerShotEffectMgr::Generate(const KazMath::Vec3<float>* arg_refPlayerPoint, shared_ptr<IEnemy> arg_refEnemy)
{

	for (auto& index : m_effects) {

		if (index.m_isActive) continue;

		index.Generate(arg_refPlayerPoint, arg_refEnemy);

		break;

	}

}

void PlayerShotEffectMgr::Update()
{
	for (auto& index : m_effects) {

		if (!index.m_isActive) continue;

		index.Update();

	}
}

void PlayerShotEffectMgr::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{
	for (auto& index : m_effects) {

		if (!index.m_isActive) continue;

		index.Draw(arg_rasterize, arg_blasVec);

	}
}
