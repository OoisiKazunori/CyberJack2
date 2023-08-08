#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Render/KazRender.h"
#include<array>
#include"../KazLibrary/Render/DrawFunc.h"
#include"../KazLibrary/Buffer/DrawFuncData.h"
#include"../KazLibrary/Render/DrawingByRasterize.h"


class PlayerHpUi
{
public:
	PlayerHpUi();

	void Init(int HP_MAX);
	void Update();
	void Draw(DrawingByRasterize& arg_rasterize);

	void Sub();
	int hp;
private:
	int maxHp;
	int prevHp;
	bool disappearFlag;
	int disappearTimer;
	int subRedHpNum;

	KazMath::Vec2<float> basePos;
	std::array<DrawFunc::TextureRender, 5> hpTex;
	std::array<DrawFunc::TextureRender, 5> redHpTex;
	std::array<KazMath::Vec2<float>, 5> lerpScale;
	std::array<int, 5> timer;
	std::array<bool, 5> lerpFlag;
	std::array<DrawFunc::TextureRender, 5> hpFlame;
	DrawFunc::TextureRender hpBackGround;

	int damageTimer;
	bool subFlag;

	int redHpUiNum;

	std::array<KazMath::Vec2<float>, 5>baseScale;

};
