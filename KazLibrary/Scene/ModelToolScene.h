#pragma once
#include"../KazLibrary/Scene/SceneBase.h"
#include"../Game/Helper/CameraWork.h"
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include"../Game/Tool/ModelTool.h"
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
	CameraWork m_camera;
	DebugCamera m_debugCamera;

	//モデル確認用のツール
	ModelTool m_modelTool;

};

