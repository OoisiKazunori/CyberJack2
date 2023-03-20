#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Math/KazMath.h"
#include"../KazLibrary/Render/KazRender.h"
#include"LineCpuEffect.h"
#include<array>

class PortalFlame
{
public:
	PortalFlame();

	void Init(const KazMath::Vec3<float> &POS, const KazMath::Vec2<float> &SIZE);
	void Update();
	void Draw();

	void Start();
	bool Flame();
	KazMath::Vec3<float>basePos;
private:

	bool startFlag;

	static const int FLAME_MAX_NUM = 4;
	//フレーム-------------------------
	std::array<LineRender, FLAME_MAX_NUM>flame;
	std::array<RESOURCE_HANDLE, FLAME_MAX_NUM>constBufferHandle;
	std::array<DirectX::XMFLOAT4, FLAME_MAX_NUM>lineEffectData;
	int flameFlashIndex;
	int prevFlameFlashIndex;
	std::array<int, FLAME_MAX_NUM>flashTimer;
	std::array<KazMath::Vec3<float>, 4>initFlamePos;
	int flameIndex;
	int flameTimer;
	int makeFlameMaxTimer;

	bool readyToFlashFlag;
	int flashMaxTimer;
	enum
	{
		LEFT_UP,
		LEFT_DOWN,
		RIGHT_UP,
		RIGHT_DOWN,
	};
	//フレーム-------------------------


	//メモリ線-------------------------
	static const int LINE_CPU_NUM = 7;
	static const int LINE_CPU_UPDOWN_NUM = LINE_CPU_NUM * 2;
	std::array<std::array<LineCpuEffect, LINE_CPU_NUM>, FLAME_MAX_NUM / 2>sideMemoryine;
	std::array<std::array<LineCpuEffect, LINE_CPU_NUM * 2>, FLAME_MAX_NUM / 2>upDownMemoryine;
};

