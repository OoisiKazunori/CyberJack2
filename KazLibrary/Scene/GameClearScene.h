#pragma once
#include"../KazLibrary/Scene/SceneBase.h"
#include"../KazLibrary/Render/DrawFunc.h"

class GameClearScene
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
};

