#pragma once
#include"../Game/Scene/SceneBase.h"
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include"Tool/ModelTool.h"
#include"../Game/Debug/DebugCamera.h"

class ModelToolScene :public SceneBase
{
public:
	ModelToolScene();
	~ModelToolScene();

	void Init();
	void PreInit();
	void Finalize();
	void Input();
	void Update();
	void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec);

	int SceneChange();


private:
	DebugCamera m_debugCamera;

	//モデル確認用のツール
	ModelTool m_modelTool;

};

