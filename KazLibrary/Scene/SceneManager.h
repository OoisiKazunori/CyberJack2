#pragma once
#include"../DirectXCommon/Base.h"
#include"../Scene/SceneBase.h"
#include"TitleScene.h"
#include"GameScene.h"
#include"DebugScene.h"
#include"../Scene/SceneChange.h"
#include"../RenderTarget/GaussianBuler.h"
#include"../RenderTarget/RenderTargetStatus.h"
#include<memory>


class SceneManager
{
public:
	SceneManager();
	~SceneManager();
	void Update();
	void Draw();

	bool endGameFlag;
private:
	std::vector<std::unique_ptr<SceneBase>> scene;
	ChangeScene::SceneChange change;
	int nowScene, nextScene;
	bool initGameFlag;

	bool sceneChangeFlag;

	bool itisInArrayFlag;
	bool gameFirstInitFlag;
};