#include "../Easing/easing.h"


//関数のポインタ
static float (*FuncPtr[])(float t) = { InLinear, InSmooth, InQuad, InCubic, InQuart, InQuint, InSine, InCirc, InExp, InBack, InSoftBack, InElastic, InBounce };

//EasingType...In,Out,InOut、EaseInType...InLinear,InSmooth...、t...変化率
float EasingMaker(int ChooseType, int ChooseEase, float t) {

	if (ChooseType == In)
	{
		return FuncPtr[ChooseEase](t);
	}
	else if (ChooseType == Out)
	{
		return 1.0f - FuncPtr[ChooseEase](1.0f - t);
	}
	else if (ChooseType == InOut)
	{
		if (t < 0.5f)
		{
			return FuncPtr[ChooseEase](2.0f * t) / 2.0f;
		}
		else
		{
			return 1.0f - FuncPtr[ChooseEase](2.0f - 2.0f * t) / 2.0f;
		}
	}
	else
	{
		return -1.0f;
	}
}


//EaseInの種類

float InLinear(float t)
{
	return t;
}
float InSmooth(float t)
{
	return t * t * (3.0f - t) / 2.0f;
}
float InQuad(float t)
{
	return t * t;
}
float InCubic(float t)
{
	return t * t * t;
}
float InQuart(float t)
{
	return t * t * t * t;
}
float InQuint(float t)
{
	return t * t * t * t * t;
}
float InSine(float t)
{
	return 1.0f - static_cast<float>(cos(t * 3.14f / 2.0f));
}
float InCirc(float t)
{
	return 1.0f - static_cast<float>(sqrt(Max(0, 1.0f - t * t)));
}
float InExp(float t)
{
	return static_cast<float>(pow(2.0f, -(1.0f - t) * 10.0f));
}
float InBack(float t)
{
	return t * t * (2.70158f * t - 1.70158f);
}
float InSoftBack(float t)
{
	return t * t * (2.0f * t - 1.0f);
}
float InElastic(float t)
{
	return 56.0f * t * t * t * t * t - 105.0f * t * t * t * t + 60.0f * t * t * t - 10.0f * t * t;
}
float InBounce(float t)
{
	float pow2, bounce = 4.0f;

	while (t < ((pow2 = static_cast<float>(pow(2.0f, --bounce))) - 1.0f) / 11.0f)
	{
	}

	return static_cast<float>(1.0f / pow(4.0f, 3.0f - bounce) - 7.5625f * pow((pow2 * 3.0f - 2.0f) / 22.0f - t, 2.0f));
}


float Max(float a, float b)
{
	if (a > b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

void Rate(float *T, float RATE, float MAX_RATE)
{
	if (*T < MAX_RATE)
	{
		*T += RATE;
	}
	else if (MAX_RATE <= *T)
	{
		*T = MAX_RATE;
	}
}
