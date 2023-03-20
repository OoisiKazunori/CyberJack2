#include "MisileSmokeEmitter.h"

MisileSmokeEmitter::MisileSmokeEmitter()
{
	initFlag = false;
}

void MisileSmokeEmitter::Init(KazMath::Vec3<float> *EMITT_POS)
{
	particleIndex = 0;
	pos = EMITT_POS;
	intervalNum = 0;
	initFlag = true;
}

void MisileSmokeEmitter::Finalize()
{
	initFlag = false;
}

void MisileSmokeEmitter::Update()
{
	if (initFlag)
	{
		++intervalNum;
		if (10 <= intervalNum)
		{
			intervalNum = 0;
			++particleIndex;
		}
		if (smokeParticle.size() <= particleIndex)
		{
			particleIndex = static_cast<int>(smokeParticle.size() - 1);
		}

		if (!smokeParticle[particleIndex].IsAlive())
		{
			smokeParticle[particleIndex].Init(*pos);
		}
	}

	for (int i = 0; i < smokeParticle.size(); ++i)
	{
		smokeParticle[i].Update();
	}
}

void MisileSmokeEmitter::Draw()
{
	for (int i = 0; i < smokeParticle.size(); ++i)
	{
		smokeParticle[i].Draw();
	}
}
