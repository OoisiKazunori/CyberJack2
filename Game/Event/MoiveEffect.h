#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Render/KazRender.h"

class MoiveEffect
{
public:
	MoiveEffect();
	void Init();
	void Update();
	void Draw();


	bool startFlag;
private:
	Sprite2DRender topTex, buttomTex;//上下にテクスチャを描画する　←　ポリゴンに変更

	KazMath::Vec2<float> lerpTopPos, lerpButtomPos;//座標
};

