#pragma once
#include"../KazLibrary/Scene/SceneBase.h"
#include"../KazLibrary/Render/DrawFunc.h"

class GameOverScene
{
public:
	GameOverScene();
	void Init();
	void PreInit();
	void Finalize();
	void Input();
	void Update();
	void Draw(DrawingByRasterize& arg_rasterize);
	int SceneChange();

private:
	DrawFuncData::DrawCallData m_clearTexDraw;
	KazBufferHelper::BufferData m_texBuffer;
	KazMath::Transform2D m_clearTrasform;

	int m_sceneNum;
};

