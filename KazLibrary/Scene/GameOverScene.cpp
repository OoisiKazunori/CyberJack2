#include "GameOverScene.h"
#include"../Game/Input/Input.h"
#include"../Helper/ResourceFilePass.h"

GameOverScene::GameOverScene() :m_sceneNum(-1),
m_clearRender(KazFilePathName::GameOverPath + "GameOver.png"),
m_inputRender(KazFilePathName::TitlePath + "Start.png"),
m_flashTimer(0),
m_flashFlag(false)
{
	m_clearRender.m_trasform.pos = { WIN_X / 2.0f, WIN_Y / 2.0f };
	m_inputRender.m_trasform.pos = { WIN_X / 2.0f, WIN_Y / 2.0f + 200.0f };
	m_inputRender.m_trasform.scale *= 0.5f;
	endGameFlag = false;
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
	//ƒ^ƒCƒgƒ‹‚É–ß‚é
	if (Input::Instance()->Done())
	{
		m_sceneNum = 0;
	}
}

void GameOverScene::Update()
{
	++m_flashTimer;
	if (KazMath::ConvertSecondToFlame(1) <= m_flashTimer)
	{
		m_flashTimer = 0;
		m_flashFlag = !m_flashFlag;
	}
}

void GameOverScene::Draw(DrawingByRasterize& arg_rasterize)
{
	DrawFunc::DrawTextureIn2D(m_clearRender.m_drawCommand, m_clearRender.m_trasform, m_clearRender.m_textureBuffer);
	arg_rasterize.ObjectRender(m_clearRender.m_drawCommand);

	if (m_flashFlag)
	{
		DrawFunc::DrawTextureIn2D(m_inputRender.m_drawCommand, m_inputRender.m_trasform, m_inputRender.m_textureBuffer);
		arg_rasterize.ObjectRender(m_inputRender.m_drawCommand);
	}
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
