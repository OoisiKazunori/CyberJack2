#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Render/KazRender.h"
#include"../Event/WindowFlame.h"

class DamageTextWindow
{
public:
	DamageTextWindow();
	void Init(const KazMath::Vec3<float> &POS);
	void Finalize();
	void Update();
	void Draw();

	bool IsEnd();

private:
	std::unique_ptr<WindowFlame> flame;
	int appearTimer;
	static const int DISAPPER_MAX_TIME;
	static const int DRAW_MAX_TIME;

	KazMath::Vec3<float> pos;

	Sprite2DRenderPtr damageTextR;
	bool initFlag;
};
