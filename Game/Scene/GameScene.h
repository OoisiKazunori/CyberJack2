#pragma once
#include"../Game/Scene/SceneBase.h"

class GameScene :public SceneBase
{
public:
	GameScene();
	~GameScene();

	void Init();
	void PreInit();
	void Finalize();
	void Input();
	void Update();
	void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec);

	int SceneChange();

private:


};