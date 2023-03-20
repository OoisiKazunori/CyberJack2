#include"StringWindow.h"

StringWindow::StringWindow()
{
	endFlag = false;
	initFlag = false;
}

void StringWindow::Init(const std::string &TEXT)
{
	KazMath::Transform3D lTrans;
	lTrans.pos = { WIN_X / 2.0f,100.0f,0.0f };
	KazMath::Vec2<float> lScale;
	lScale = { 3.0f,0.8f };
	window.Init(lTrans, lScale, false, WINDOW_2D);

	endFlag = false;
	endTimer = 0;
	window.Start();
	readyToWriteFlag = false;
	initFlag = true;

	text = TEXT;
}

void StringWindow::Finalize()
{
	tutorialText.Finalize();
	window.End();
	initFlag = false;
}

void StringWindow::Update()
{
	if (!initFlag)
	{
		return;
	}

	if (window.ReadyToWrite() && !readyToWriteFlag)
	{
		tutorialText.Init({ WIN_X / 2.0f - 158.0f,100.0f }, text, 1.4f);
		readyToWriteFlag = true;
	}

	tutorialText.Update(0);
	window.Update();

	++endTimer;
	if (60 <= endTimer)
	{
		endFlag = true;
	}

	if (!endFlag)
	{
		return;
	}

	if (KazMath::ConvertSecondToFlame(2) <= endTimer)
	{
		window.End();
		tutorialText.Finalize();
	}
}

void StringWindow::Draw()
{
	if (!initFlag)
	{
		return;
	}
	tutorialText.Draw();
	window.Draw();
}

bool StringWindow::IsFinish()
{
	return window.IsEnd();
}
