#include "EaseScaleParticle.h"
#include "../KazLibrary/Helper/ResourceFilePass.h"

const float EaseScaleParticle::MAX_RATE = 1.0f;

EaseScaleParticle::EaseScaleParticle(int GRAPH_HANDLE) :initFlag(false)
{
	render.data.handleData = GRAPH_HANDLE;
	render.data.pipelineName = PIPELINE_NAME_SPRITE_Z_OFF;
}

void EaseScaleParticle::Init(const KazMath::Vec2<float> &POS, const KazMath::Vec2<float> &BASE_SCALE, const KazMath::Vec2<float> &EASE_SCALE, const KazMath::Vec2<int> &TIMER, bool ROTA_FLAG)
{
	baseScale.x = BASE_SCALE.x;
	baseScale.y = BASE_SCALE.y;

	easeScale.x = EASE_SCALE.x;
	easeScale.y = EASE_SCALE.y;

	render.data.transform.pos = POS;

	easeMaxTimer.x = static_cast<float>(TIMER.x);
	easeMaxTimer.y = static_cast<float>(TIMER.y);

	render.data.transform.scale = baseScale;
	render.data.transform.rotation = 0.0f;

	render.data.colorData.color.a = 255;

	rate = { 0.0f,0.0f };
	initFlag = true;
	rotaFlag = ROTA_FLAG;
}

void EaseScaleParticle::Update()
{
	if (initFlag)
	{
		Rate(&rate.x, MAX_RATE / easeMaxTimer.x, MAX_RATE);
		Rate(&rate.y, MAX_RATE / easeMaxTimer.y, MAX_RATE);

		render.data.transform.scale.x = baseScale.x + EasingMaker(Out, Cubic, rate.x) * easeScale.x;
		render.data.transform.scale.y = baseScale.y + EasingMaker(Out, Cubic, rate.y) * easeScale.y;

		if (0 < render.data.colorData.color.a)
		{
			render.data.colorData.color.a -= 5;
		}
		else
		{
			render.data.colorData.color.a = 0;
			initFlag = false;
		}

		if (rotaFlag)
		{
			render.data.transform.rotation += 5.0f;
		}
	}
}

void EaseScaleParticle::Draw()
{
	if (initFlag)
	{
		render.Draw();
	}
}

bool EaseScaleParticle::IsAlive()
{
	return initFlag;
}
