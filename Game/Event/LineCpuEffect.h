#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Math/KazMath.h"
#include"../KazLibrary/Render/KazRender.h"
#include<array>
#include"../Game/Debug/WirteCpuLineData.h"

enum LineEffectVec
{
	LINE_UPVEC,
	LINE_DOWNVEC,
	LINE_LEFTVEC,
	LINE_RIGHTVEC,
};

class LineCpuEffect
{
public:
	LineCpuEffect();

	void Init(LineEffectVec VEC, KazMath::Vec3<float> &POS);
	void Update();
	void Draw();

	float rate;
	void FlashLight();
	void InitFlash();

	void Appear();

	KazMath::Vec3<float>basePos;

private:
	static int LINE_ID;
	int id;
	static const int LINE_MAX_NUM = 1;
	std::array<LineRender, LINE_MAX_NUM>lineRender;
	
	std::array<RESOURCE_HANDLE, LINE_MAX_NUM>constBufferHandle;
	std::array<DirectX::XMFLOAT4, LINE_MAX_NUM>lineEffectData;
	std::array<int, LINE_MAX_NUM>flashTimer;
	int maxTimer;
	bool flashFlag;

	CircleRender circleRender;
	std::string name;

	KazMath::Vec3<float>endPos;

	int circleAppearTimer;
	int circleMaxAppearTimer;

	//ìoèÍââèo
	int appearTimer;
	int maxAppearTimer;
	bool startToAppearFlag;

};