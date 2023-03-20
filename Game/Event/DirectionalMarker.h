#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Math/KazMath.h"
#include"../KazLibrary/Render/KazRender.h"

/// <summary>
/// ‘ÎÛ‚ÉŒü‚©‚Á‚Ä–îˆó‚Ì•`‰æ
/// </summary>
class DirectionalMarker
{
public:
	DirectionalMarker();
	void Init(const KazMath::Vec3<float> &OBJ_POS);
	void Update();
	void Draw();

private:
	KazMath::Vec3<float>objPos;
	Sprite3DRender dir3DRender;
	Sprite2DRender dir2DRender;
	bool isInScreenFlag;
	bool initFlag;

	KazMath::Vec2<float> shrinkScaleRate;
	float timer;
};

