#include "StageSelectScene.h"

StageSelectScene::StageSelectScene()
{
	_stageNumber = 0;
}

void StageSelectScene::Init()
{
	_sceneChange = false;
	_titleBackFlag = false;
	_titleSceneFlag = false;

	_timeFlag = false;
	_timer = 0;

}

void StageSelectScene::Finalize()
{
}

void StageSelectScene::Update()
{
	
}

void StageSelectScene::Draw() const
{
}

void StageSelectScene::Input(bool Left, bool Right, bool Done)
{
	leftFlag = Left;
	rightFlag = Right;
	doneFlag = Done;
}

int StageSelectScene::StageNumber()
{
	return _stageNumber;
}

bool StageSelectScene::SceneChange()
{
	return _sceneChange;
}

bool StageSelectScene::TitleBack()
{
	return _titleSceneFlag;
}
