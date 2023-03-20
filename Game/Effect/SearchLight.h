#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../KazLibrary/Loader/TextureResourceMgr.h"

class SearchLight
{
public:
	SearchLight();
	void Init(const KazMath::Vec3<float> &POS, const KazMath::Color &COLOR);
	void Update();
	void Draw();

private:
	Sprite3DRender lightTextureR;
	float sinTimer;
	float sinTimeVel;
	float length;
};