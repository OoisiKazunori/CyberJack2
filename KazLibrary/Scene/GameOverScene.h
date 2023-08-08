#pragma once
#include"../KazLibrary/Scene/SceneBase.h"

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
};

