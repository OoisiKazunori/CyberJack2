#include"../Scene/TitleScene.h"
#include"../DirectXCommon/Base.h"
#include"../Input/KeyBoradInputManager.h"
#include"../Input/ControllerInputManager.h"
#include"../KazLibrary/Camera/CameraMgr.h"
#include"../KazLibrary/RenderTarget/RenderTargetStatus.h"
#include"../Imgui/MyImgui.h"
#include"../Helper/ResourceFilePass.h"
#include"../Easing/easing.h"
#include"../Game/Input/Input.h"
#include"../Helper/ResourceFilePass.h"

TitleScene::TitleScene() :m_sceneNum(-1),
m_clearRender(KazFilePathName::TitlePath + "TitleName.png"),
m_inputRender(KazFilePathName::TitlePath + "Start.png"),
m_flashTimer(0),
m_flashFlag(false)
{
	m_clearRender.m_transform.pos = { WIN_X / 2.0f, WIN_Y / 2.0f };
	m_inputRender.m_transform.pos = { WIN_X / 2.0f, WIN_Y / 2.0f + 200.0f };
	m_inputRender.m_transform.scale *= 0.5f;
	endGameFlag = false;
}

void TitleScene::Init()
{
}

void TitleScene::Finalize()
{
}

void TitleScene::Input()
{
	//ƒQ[ƒ€ƒV[ƒ“‚É“ü‚é
	if (Input::Instance()->Done())
	{
		m_sceneNum = 1;
	}
}

void TitleScene::Update()
{
	++m_flashTimer;
	if (KazMath::ConvertSecondToFlame(1) <= m_flashTimer)
	{
		m_flashTimer = 0;
		m_flashFlag = !m_flashFlag;
	}
}

void TitleScene::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{
	DrawFunc::DrawTextureIn2D(m_clearRender.m_drawCommand, m_clearRender.m_transform, m_clearRender.m_textureBuffer);
	arg_rasterize.ObjectRender(m_clearRender.m_drawCommand);

	if (m_flashFlag)
	{
		DrawFunc::DrawTextureIn2D(m_inputRender.m_drawCommand, m_inputRender.m_transform, m_inputRender.m_textureBuffer);
		arg_rasterize.ObjectRender(m_inputRender.m_drawCommand);
	}
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
