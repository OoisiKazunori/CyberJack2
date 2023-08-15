#include "PlayerShotEffect.h"

PlayerShotEffect::PlayerShotEffect()
{


}

void PlayerShotEffect::Init()
{

	m_flame = 0;

}

void PlayerShotEffect::Generate(const KazMath::Vec3<float>* arg_refEnemyPos, const KazMath::Vec3<float>* arg_refPlayerPos, shared_ptr<IEnemy> arg_refEnemy)
{
	//制御点を決めるベクトルを求める。
	m_controlPointVec = KazMath::Vec3<float>(KazMath::Rand(-CONTROL_POINT_R, CONTROL_POINT_R), KazMath::Rand(-CONTROL_POINT_R, CONTROL_POINT_R), KazMath::Rand(-CONTROL_POINT_R, CONTROL_POINT_R));

	m_flame = 0;
	m_refEnemyPos = arg_refEnemyPos;
	m_refPlayerPos = arg_refPlayerPos;
	m_refEnemy = arg_refEnemy;
}

void PlayerShotEffect::Update()
{

	++m_flame;

}

void PlayerShotEffect::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{
}
