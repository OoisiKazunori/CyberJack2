#include "GameClearScene.h"

GameClearScene::GameClearScene()
{
}

void GameClearScene::Init()
{
}

void GameClearScene::PreInit()
{
}

void GameClearScene::Finalize()
{
}

void GameClearScene::Input()
{
}

void GameClearScene::Update()
{
}

void GameClearScene::Draw(DrawingByRasterize& arg_rasterize)
{
	DrawFunc::DrawTextureIn2D(sceneTex, transform, texBuffer);
	arg_rasterize.ObjectRender(sceneTex);
}

int GameClearScene::SceneChange()
{
	return 0;
}
