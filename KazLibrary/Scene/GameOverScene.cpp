#include "GameOverScene.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"

GameOverScene::GameOverScene() :m_sceneNum(-1)
{
	m_clearTexDraw = DrawFuncData::SetTexPlaneData(DrawFuncData::GetSpriteShader());
	m_texBuffer = TextureResourceMgr::Instance()->LoadGraphBuffer("");
}

void GameOverScene::Init()
{
}

void GameOverScene::PreInit()
{
}

void GameOverScene::Finalize()
{
}

void GameOverScene::Input()
{
	if (KeyBoradInputManager::Instance()->InputTrigger(DIK_SPACE))
	{
		m_sceneNum = 0;
	}
}

void GameOverScene::Update()
{
}

void GameOverScene::Draw(DrawingByRasterize& arg_rasterize)
{
	DrawFunc::DrawTextureIn2D(m_clearTexDraw, m_clearTrasform, m_texBuffer);
	arg_rasterize.ObjectRender(m_clearTexDraw);
}

int GameOverScene::SceneChange()
{
	if (m_sceneNum != -1)
	{
		int tmp = m_sceneNum;
		m_sceneNum = -1;
		return tmp;
	}
	return SCENE_NONE;
}
