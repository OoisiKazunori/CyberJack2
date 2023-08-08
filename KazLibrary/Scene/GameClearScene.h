#pragma once
#include"../KazLibrary/Scene/SceneBase.h"
#include"../KazLibrary/Render/DrawFunc.h"

class GameClearScene:public SceneBase
{
public:
	GameClearScene();
	void Init();
	void PreInit();
	void Finalize();
	void Input();
	void Update();
	void Draw(DrawingByRasterize& arg_rasterize);
	int SceneChange();

private:
	DrawFunc::TextureRender m_clearRender;
	DrawFunc::TextureRender m_inputRender;

	int m_sceneNum;
	bool m_flashFlag;
	int m_flashTimer;
};

