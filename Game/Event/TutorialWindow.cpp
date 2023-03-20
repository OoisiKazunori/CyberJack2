#include "TutorialWindow.h"
#include "../KazLibrary/Input/KeyBoradInputManager.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

TutorialWindow::TutorialWindow()
{
	lStickUi.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TutorialPath + "LStick.png");
	abuttonUi.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TutorialPath + "Abutton.png");
	attackUi.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TutorialPath + "Attack.png");

	lStickUi.data.pipelineName = PIPELINE_NAME_SPRITE_CUTALPHA;
	abuttonUi.data.pipelineName = PIPELINE_NAME_SPRITE_CUTALPHA;
	attackUi.data.pipelineName = PIPELINE_NAME_SPRITE_CUTALPHA;


	lStickUi.data.transform.pos = { 14.7f,2.0f,14.5f };
	lStickUi.data.transform.scale = { 0.18f,0.2f,1.0f };
	lStickUi.data.transform.rotation = { 0.0f,20.0f,0.0f };

	abuttonUi.data.transform.pos = { 14.7f,0.0f,14.5f };
	abuttonUi.data.transform.scale = { 0.18f,0.2f,1.0f };
	abuttonUi.data.transform.rotation = { 0.0f,20.0f,0.0f };

	attackUi.data.transform.pos = { 14.7f,-2.0f,14.5f };
	attackUi.data.transform.scale = { 0.2f,0.2f,1.0f };
	attackUi.data.transform.rotation = { 0.0f,20.0f,0.0f };

}

void TutorialWindow::Init()
{
	KazMath::Transform3D transform;
	transform.pos = { 15.0f,0.0f,15.0f };
	transform.scale = { 0.0f,0.0f,0.0f };
	transform.rotation = { 0.0f,20.0f,0.0f };
	windowFlame.Init(transform, { 0.3f,0.35f }, false, WINDOW_3D);
	windowFlame.Start();
	endFlag = false;
}

void TutorialWindow::Update()
{
	windowFlame.Update();

	line.data.startPos = { 0.0f,0.0f,0.0f };
	line.data.endPos = { 0.0f,0.0f,0.0f };
}

void TutorialWindow::Draw()
{
	line.Draw();
	if (!endFlag)
	{
		lStickUi.Draw();
		abuttonUi.Draw();
		attackUi.Draw();
	}
	windowFlame.Draw();
}

void TutorialWindow::End()
{
	endFlag = true;
	windowFlame.End();
}
