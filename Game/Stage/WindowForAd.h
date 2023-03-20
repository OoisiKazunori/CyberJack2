#pragma once
#include"../KazLibrary/Render/KazRender.h"

/// <summary>
/// 3D空間上にスプライトを配置、ウィンドウのフレーム内に画像を描画、手前に動きます
/// </summary>
class WindowForAd
{
public:
	WindowForAd();
	void Init(KazMath::Vec3<float> &POS, const std::array<RESOURCE_HANDLE, 10> &HANDLE_ARRAY);
	void Update();
	void Draw();

private:
	Sprite3DRenderPtr spriteRender;
	int timer;
	int maxTimer;
	bool exchangeWindowFlag;
	int noiseTimer;
	int noiseMaxTimer;

	RESOURCE_HANDLE noiseHandle;
	std::array<RESOURCE_HANDLE, 10>windowTexHandleArray;

	struct ObjectData
	{
		float seed;
		DirectX::XMFLOAT3 pad;
		DirectX::XMFLOAT4 fogData;
	};

	ObjectData data;
};

