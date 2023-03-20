#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Render/KazRender.h"

class GoalBoxPortalEffect
{
public:
	GoalBoxPortalEffect();

	void Init(const KazMath::Vec3<float> &POS);
	void Update();
	void Draw();

	void Start();
	bool AllHidden();
	bool DrawPrevStageFlag();
	void CheckCameraPos(float POS_Z);

	Sprite3DRenderPtr noiseSprite;
	int timer;
	bool changeStageFlag;
private:
	bool startFlag;
	bool allHiddenCalledFlag;
	bool drawPrevStageFlag;
	Sprite3DRender portalSprite;
	bool appearNextStageFlag;
	RESOURCE_HANDLE constBuffHandle;
	KazMath::Vec3<float> lerpScale;
};
