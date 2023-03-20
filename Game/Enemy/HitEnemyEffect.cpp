#include "HitEnemyEffect.h"
#include"../KazLibrary/Easing/easing.h"

HitEnemyEffect::HitEnemyEffect() :startFlag(false)
{
	for (int i = 0; i < circleR.size(); ++i)
	{
		circleR[i].data.change3DFlag = true;
		circleR[i].data.billBoardFlag = true;
		circleR[i].data.fillFlag = false;

	}


	baseRadius[BIG_CIRCLE] = 3.0f;
	baseRadius[SMALL_CIRCLE] = 0.5f;

	maxRadius = 10.0f;
	addRadius[BIG_CIRCLE] = maxRadius;
	addRadius[SMALL_CIRCLE] = (addRadius[BIG_CIRCLE] + baseRadius[BIG_CIRCLE]) - baseRadius[SMALL_CIRCLE];
	maxTimer = 60;
}

void HitEnemyEffect::Update()
{
	if (startFlag)
	{
		for (int i = 0; i < circleR.size(); ++i)
		{
			circleR[i].data.transform.pos = pos;
			circleR[i].data.radius = baseRadius[i] + EasingMaker(Out, Cubic, KazMath::ConvertTimerToRate(timer[i], maxTimer)) * addRadius[i];

			if (maxTimer <= timer[i])
			{
				startFlag = false;
			}
			else
			{
				++timer[i];
			}

			circleR[i].data.colorData.color.a -= 10;
			if (circleR[i].data.colorData.color.a <= 0)
			{
				circleR[i].data.colorData.color.a = 0;
				
			}
		}
	}
}

void HitEnemyEffect::Draw()
{
	if (startFlag)
	{
		for (int i = 0; i < circleR.size(); ++i)
		{
			circleR[i].Draw();
		}
	}
}

void HitEnemyEffect::Start(const KazMath::Vec3<float> &POS)
{
	if (!startFlag)
	{
		pos = POS;

		for (int i = 0; i < timer.size(); ++i)
		{
			timer[i] = 0;
			circleR[i].data.colorData.color.a = 255;
		}
		startFlag = true;
	}
}

bool HitEnemyEffect::IsAlive()
{
	return startFlag;
}
