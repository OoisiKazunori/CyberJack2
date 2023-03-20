#include "MisileSmokeParticle.h"

MisileSmokeParticle::MisileSmokeParticle()
{
	patricle = std::make_unique<CircleRender>();
	patricle->data.billBoardFlag = true;
	patricle->data.change3DFlag = true;
	patricle->data.radius = 3.0f;
	initFlag = false;
}

void MisileSmokeParticle::Init(const KazMath::Vec3<float> &EMITT_POS)
{
	patricle->data.transform.pos = EMITT_POS;
	patricle->data.colorData.color = { 255,255,255,255 };
	initFlag = true;
}

void MisileSmokeParticle::Finalize()
{
	initFlag = false;
}

void MisileSmokeParticle::Update()
{
	if (initFlag)
	{
		patricle->data.colorData.color.a -= 5;
		if (patricle->data.colorData.color.a < 0)
		{
			patricle->data.colorData.color.a = 0;
			initFlag = false;
		}
	}
}

void MisileSmokeParticle::Draw()
{
	if (initFlag)
	{
		patricle->Draw();
	}
}

bool MisileSmokeParticle::IsAlive()
{
	return initFlag;
}
