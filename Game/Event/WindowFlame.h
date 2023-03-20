#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Render/KazRender.h"

/// <summary>
/// ゲーム内のウィンドウのフレーム描画
/// </summary>

enum WindowType
{
	WINDOW_2D,
	WINDOW_3D
};

class WindowFlame
{
public:
	WindowFlame();

	void Init(const KazMath::Transform3D &TRANSFORM, const KazMath::Vec2<float> &SCALE, bool APPEAR_WINDOW, WindowType TYPE);
	void Update();
	void Draw();

	void Start();
	void End();

	bool IsEnd();
	bool ReadyToWrite();

	KazMath::Vec3<float> basePos;
private:
	bool startFlag;				//アナウンスを開始します
	bool endFlag;
	bool drawFlag;
	bool readyToWriteFlag;
	int stageNum;				//現在のステージ番号を表示します

	KazMath::Vec3<float> larpScale;

	std::array<int, 10> numberHandle;
	Sprite3DRender flameTex;	//ウィンドウの描画
	Sprite2DRender flame2DTex;	//ウィンドウの描画

	KazMath::Vec2<float> lerpMaxScale;

	WindowType windowType;

	void Update2D();
	void Update3D();
};

