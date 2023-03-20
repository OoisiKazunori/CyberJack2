#pragma once
#include"../Interface/IOperationObject.h"
#include"../KazLibrary/Render/KazRender.h"

/// <summary>
/// スプライト系に当たり判定を付ける
/// </summary>
class EventSprite :public IOperationObject
{
public:

	EventSprite();

	void Init(const KazMath::Vec3<float> &POS, int HANDLE);
	void Update();
	void Draw();

	Sphere hitBox;
	bool selectingFlag;
	Sprite3DRender spriteRender;
private:
	bool hitFlag;
	bool deadFlag;
	int timer;

	RESOURCE_HANDLE hitHandle, selectHandle, noHitHandle;
};

