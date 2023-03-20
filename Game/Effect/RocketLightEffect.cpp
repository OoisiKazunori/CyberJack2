#include "RocketLightEffect.h"

RocketLightEffect::RocketLightEffect()
{
	lightEffectRender.data.change3DFlag = true;
	lightEffectRender.data.billBoardFlag = true;
	lightEffectRender.data.radius = 10.0f;
	lightEffectRender.data.colorData.color = { 248, 58, 16 ,255 };
	initFlag = false;


	lightEffectRender.data.pipelineName = PIPELINE_NAME_COLOR_MULTITEX;
	RESOURCE_HANDLE lHandle = lightEffectRender.CreateConstBuffer(sizeof(DirectX::XMFLOAT4), typeid(DirectX::XMFLOAT4).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA);
	DirectX::XMFLOAT4 lColor(1.0f, 0.0, 0.0, 1.0f);
	lightEffectRender.TransData(&lColor, lHandle, typeid(DirectX::XMFLOAT4).name());
}

void RocketLightEffect::Init(KazMath::Vec3<float> *BASE_POS_PTR, const KazMath::Vec3<float> &TEX_POS, bool APPEAR_FLAG, bool *IS_DEAD_FLAG, float *RADIUS, bool *START_FLAG)
{
	pos = BASE_POS_PTR;
	bloomTexPos = TEX_POS;
	flashTimer = 0;
	if (APPEAR_FLAG)
	{
		lightEffectRender.data.colorData.color.a = 0;
	}
	else
	{
		lightEffectRender.data.colorData.color.a = 255;
	}
	radius = RADIUS;
	initFlag = START_FLAG;
	deadFlag = IS_DEAD_FLAG;
}

void RocketLightEffect::Update()
{
	if (initFlag != nullptr && *initFlag)
	{
		if (lightEffectRender.data.colorData.color.a < 255)
		{
			lightEffectRender.data.colorData.color.a += 5;
		}
		else
		{
			lightEffectRender.data.colorData.color.a = 255;
		}

		if (3 <= flashTimer)
		{
			flashTimer = 0;
			lightEffectRender.data.radius = *radius;
		}
		else
		{
			lightEffectRender.data.radius = *radius + 2.0f;
			++flashTimer;
		}

		lightEffectRender.data.transform.pos = {};
		lightEffectRender.data.transform.pos = *pos + bloomTexPos;


		if (!*deadFlag)
		{
			*initFlag = false;
		}
	}
}

void RocketLightEffect::Draw()
{
	if (initFlag != nullptr && *initFlag)
	{
		lightEffectRender.Draw();
	}
}

bool RocketLightEffect::IsStart()
{
	if (initFlag != nullptr)
	{
		return *initFlag;
	}
	return false;
}

void RocketLightEffect::Fisnish()
{
	if (initFlag != nullptr)
	{
		*initFlag = false;
	}
}