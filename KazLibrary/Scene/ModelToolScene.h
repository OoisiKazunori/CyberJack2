#pragma once
#include"../KazLibrary/Scene/SceneBase.h"
#include"../Game/Helper/CameraWork.h"
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include"../Game/Tool/ModelTool.h"

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
	void Draw();

	int SceneChange();


private:
	CameraWork m_camera;
	DrawingByRasterize m_rasterizeRenderer;

	//モデル確認用のツール
	ModelTool m_modelTool;

};

