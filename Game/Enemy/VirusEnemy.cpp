#include "VirusEnemy.h"

VirusEnemy::VirusEnemy()
{
}

void VirusEnemy::Init(const KazMath::Transform3D* arg_playerTransform, const EnemyGenerateData& GENERATE_DATA, bool DEMO_FLAG)
{
	iEnemy_EnemyStatusData->oprationObjData->Init(1, "Virus");
	m_playerTransform = arg_playerTransform;
}

void VirusEnemy::Finalize()
{
}

void VirusEnemy::Update()
{
}

void VirusEnemy::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{
	bool debug = false;
}
