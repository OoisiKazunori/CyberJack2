#include "VirusEnemy.h"

VirusEnemy::VirusEnemy()
{
}

void VirusEnemy::Init(const EnemyGenerateData& GENERATE_DATA, bool DEMO_FLAG)
{
	iEnemy_EnemyStatusData->oprationObjData->Init(1, "Virus");
}

void VirusEnemy::Finalize()
{
}

void VirusEnemy::Update()
{
}

void VirusEnemy::Draw(DrawingByRasterize& arg_rasterize)
{
	bool debug = false;
}
