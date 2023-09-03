#include"../Scene/TitleScene.h"

TitleScene::TitleScene()
{
}

void TitleScene::Init()
{
}

void TitleScene::Finalize()
{
}

void TitleScene::Input()
{
}

void TitleScene::Update()
{
}

void TitleScene::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{
}

int TitleScene::SceneChange()
{
	if (m_sceneNum != -1)
	{
		int tmp = m_sceneNum;
		m_sceneNum = -1;
		return tmp;
	}
	return SCENE_NONE;
}
