#pragma once
#include"../KazLibrary/Render/KazRender.h"

/// <summary>
/// チュートリアルで表示するゲージ
/// </summary>
class Gauge
{
public:
	Gauge(RESOURCE_HANDLE FLAME_TEXTURE_HANDLE, RESOURCE_HANDLE GAUGE_TEXTURE_HANDLE);
	void Init(int MAX);

	void Update();
	void Draw();

	void Add(int NUM);
	bool IsMax();

private:
	int maxNum;
	int nowNum;
	bool isMaxFlag, isHalfFlag;
	float rate;

	Sprite2DRender flameR, gaugeR;

	const KazMath::Color NORMAL_COLOR, HALF_COLOR, MAX_COLOR;
};

