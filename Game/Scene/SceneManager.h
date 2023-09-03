#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../Scene/SceneBase.h"
#include"TitleScene.h"
#include"GameScene.h"
#include"DemoScene.h"
#include"../Scene/SceneChange.h"
#include"RenderTarget/GaussianBuler.h"
#include"RenderTarget/RenderTargetStatus.h"
#include<memory>
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include "../KazLibrary/Sound/SoundManager.h"
#include"../KazLibrary/Raytracing/RayPipeline.h"


class SceneManager
{
public:
	SceneManager();
	~SceneManager();
	void Update();
	void Draw();

	bool endGameFlag;
private:
	std::vector<std::unique_ptr<SceneBase>> scene;
	std::unique_ptr<ChangeScene::SceneChange> change;
	int nowScene, nextScene;
	bool initGameFlag;

	bool sceneChangeFlag;

	bool itisInArrayFlag;
	bool gameFirstInitFlag;


	SoundData m_BGN;


	//デバッグ関連の変数
	bool m_isDebugCamera;
	bool m_isDebugRaytracing;
	bool m_isDebugTimeZone;
	bool m_isDebugVolumeFog;
	bool m_isDebugSea;


	//ラスタライザ描画
	DrawingByRasterize m_rasterize;

	//レイトレにモデルを組み込む用の配列クラス
	Raytracing::BlasVector m_blasVector;
	//レイトレで描画するための情報
	Raytracing::Tlas m_tlas;
	//レイトレ用パイプライン
	std::vector<Raytracing::RayPipelineShaderData> m_pipelineShaders;
	std::unique_ptr<Raytracing::RayPipeline> m_rayPipeline;
};