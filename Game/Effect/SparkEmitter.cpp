#include "SparkEmitter.h"

SparkEmitter::SparkEmitter()
{
	initFlag = false;
}

void SparkEmitter::Init(const KazMath::Vec3<float> *POS)
{
	pos = POS;
	initFlag = true;
	timer = 0;
	particleIndex = 0;
}

void SparkEmitter::Finalize()
{
}

void SparkEmitter::Update()
{
	if (initFlag)
	{
		if (5 < timer)
		{
			++particleIndex;
			timer = 0;
		}
		++timer;

		if (particle.size() <= particleIndex)
		{
			particleIndex = 0;
		}
		if (!particle[particleIndex].IsActive())
		{
			particle[particleIndex].Init(*pos, KazMath::Rand<int>(160, 200));
		}
		for (int i = 0; i < particle.size(); ++i)
		{
			particle[i].Update();
		}
	}
}

void SparkEmitter::Draw()
{
	if (initFlag)
	{
		for (int i = 0; i < particle.size(); ++i)
		{
			particle[i].Draw();
		}
	}
}
