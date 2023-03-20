#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Render/Sprite2DRender.h"

/// <summary>
/// それぞれのステージが切り替わった際に現在のステージ数を知らせるUI
/// </summary>
class AnnounceStageUI
{
public:
	AnnounceStageUI();

	void Init();
	void Update();
	void Draw();

	void AnnounceStage(int STAGE_NUM);
private:
	bool startFlag;				//アナウンスを開始します
	bool endFlag;
	int stageNum;				//現在のステージ番号を表示します

	KazMath::Vec2<float> basePos, larpScale;

	int areaNum;
	int areaTimer;

	std::array<RESOURCE_HANDLE, 10> numberHandle;
	Sprite2DRender flameTex;	//ウィンドウの描画

	std::array<Sprite2DRender, 6> areaTex;		//エリア文字の描画

	int timer;
};

