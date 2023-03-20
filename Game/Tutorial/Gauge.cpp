#include "Gauge.h"

Gauge::Gauge(RESOURCE_HANDLE FLAME_TEXTURE_HANDLE, RESOURCE_HANDLE GAUGE_TEXTURE_HANDLE) :
	NORMAL_COLOR(KazMath::Color(0, 255, 0, 255)), HALF_COLOR(KazMath::Color(255, 255, 0, 255)), MAX_COLOR(KazMath::Color(255, 0, 0, 255))
{
	flameR.data.handleData = FLAME_TEXTURE_HANDLE;
	gaugeR.data.handleData = GAUGE_TEXTURE_HANDLE;


	flameR.data.transform.pos = { WIN_X / 2.0f,WIN_Y - 100.0f };
	flameR.data.transform.scale = { 10.0f,3.0f };
	gaugeR.data.transform.pos = { WIN_X / 2.0f - 500.0f,WIN_Y - 100.0f };
	gaugeR.data.transform.scale = { 1.0f,55.0f };

	flameR.data.pipelineName = PIPELINE_NAME_SPRITE_COLOR;
	gaugeR.data.pipelineName = PIPELINE_NAME_SPRITE_COLOR;

	isMaxFlag = false;
	isHalfFlag = false;
}

void Gauge::Init(int MAX)
{
	maxNum = MAX;
	nowNum = 0;
	isMaxFlag = false;
	isHalfFlag = false;

	gaugeR.data.colorData.color = NORMAL_COLOR.color;
	flameR.data.colorData.color = NORMAL_COLOR.color;
}

void Gauge::Update()
{
	rate = static_cast<float>(nowNum) / static_cast<float>(maxNum);

	if (1.0f <= rate)
	{
		rate = 1.0f;
		gaugeR.data.colorData.color = MAX_COLOR.color;
		flameR.data.colorData.color = MAX_COLOR.color;
		isMaxFlag = true;
	}
	else if (0.5f <= rate)
	{
		isHalfFlag = true;
		gaugeR.data.colorData.color = HALF_COLOR.color;
		flameR.data.colorData.color = HALF_COLOR.color;
	}

	float lMaxRate = 2000.0f;
	gaugeR.data.rightScale.x = rate * lMaxRate;
}

void Gauge::Draw()
{
	gaugeR.Draw();
	flameR.Draw();
}

void Gauge::Add(int NUM)
{
	nowNum += NUM;
}

bool Gauge::IsMax()
{
	return isMaxFlag;
}
