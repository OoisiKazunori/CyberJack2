#include"../Scene/TitleScene.h"
#include"../DirectXCommon/Base.h"
#include"../Input/KeyBoradInputManager.h"
#include"../Input/ControllerInputManager.h"
#include"../KazLibrary/Camera/CameraMgr.h"
#include"../KazLibrary/RenderTarget/RenderTargetStatus.h"
#include"../Imgui/MyImgui.h"
#include"../Helper/ResourceFilePass.h"
#include"../Easing/easing.h"
#include"../Game/Event/GameCommonData.h"

TitleScene::TitleScene() :movie(true)
{
	mainRenderTargetRender.data.transform.pos = { WIN_X / 2.0f,WIN_Y / 2.0f };
	mainRenderTargetRender.data.pipelineName = PIPELINE_NAME_SPRITE_NOBLEND;

	gridR[0] = std::make_unique<DrawGrid>(KazMath::Color(14, 12, 13, 255));
	gridR[1] = std::make_unique<DrawGrid>(KazMath::Color(14, 12, 13, 255));

	mainGridR[0] = std::make_unique<DrawGrid>(KazMath::Color(14, 12, 13, 255));
	mainGridR[1] = std::make_unique<DrawGrid>(KazMath::Color(14, 12, 13, 255));

	CameraMgr::Instance()->CameraSetting(60.0f, 10000.0f, 0);


	titleLogoR.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TitlePath + "TitleName.png");
	lineLogoR.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::UIPath + "Line.png");
	startButtonR.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TitlePath + "Start.png");

	titleLogoR.data.transform.pos = { 550.0f,260.0f };
	lineLogoR.data.transform.pos = { 700.0f,355.0f };
	lineLogoR.data.transform.scale = { 1000.0f,0.5f };
	startButtonR.data.transform.pos = { 270.0f,380.0f };
	startButtonR.data.transform.scale = { 0.5f,0.5f };

	titleLogoR.data.pipelineName = PIPELINE_NAME_SPRITE_CUTALPHA;
	startButtonR.data.pipelineName = PIPELINE_NAME_SPRITE_CUTALPHA;


	endGameFlag = false;

	player.Init({ 0.0f,0.0f,4.5f }, false, true);

	onlyControllerR.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::UIPath + "OnlyController.png");
	onlyControllerR.data.transform.pos = { 1000.0f,380.0f };

	renderTarget = std::make_unique<GameRenderTarget>(KazMath::Color(14, 12, 13, 255));
	mainRenderTarget = std::make_unique<GameRenderTarget>(KazMath::Color(14, 12, 13, 255));

	isFirstFlag = false;
}

TitleScene::~TitleScene()
{
}

void TitleScene::Init()
{
	sceneNum = -1;
	cameraIndex = 0;

	KazMath::Color lBaseColor(155, 155, 155, 155);
	KazMath::Color lFlashColor(155, 155, 155, 255);
	std::array<KazMath::Color, 2>lFlashColorArray = { lBaseColor,lFlashColor };
	gridR[0]->Init(true, 800.0f, -200.0f, &cameraIndex, false, true, lFlashColorArray, false);
	gridR[1]->Init(true, 800.0f, -200.0f, &cameraIndex, false, true, lFlashColorArray, false);
	mainGridR[0]->Init(true, 800.0f, -200.0f, &cameraIndex, false, true, lFlashColorArray, true);
	mainGridR[1]->Init(true, 800.0f, -200.0f, &cameraIndex, false, true, lFlashColorArray, true);

	buttonTimer = 0;
	buttonFlashFlag = true;

	selectMenu.Init(MENU_MAX_NUM - 1);

	basePos[0] = { 150.0f,375.0f };
	basePos[1] = { 150.0f,380.0f };
	pressStartButtonFlag = false;

	startGameFlag = false;

	for (int i = 0; i < playerAroundCircleR.size(); ++i)
	{
		playerAroundCircleR[i].data.change3DFlag = true;
		playerAroundCircleR[i].data.radius = 0.0f;
		playerAroundCircleR[i].data.transform.pos = { 0.0f,0.0f,15.0f };
		playerAroundCircleR[i].data.fillFlag = false;
	}
	playerAroundCircleR[0].data.transform.rotation = { 50.0f,50.0f,50.0f };
	playerAroundCircleR[1].data.transform.rotation = { 10.0f,10.0f,10.0f };

	std::array< KazMath::Transform3D, 4> lTransform;
	lTransform[0].pos = { 250.0f,200.0f,0.0f };
	lTransform[1].pos = { 300.0f,550.0f,0.0f };
	lTransform[2].pos = { 1000.0f,150.0f,0.0f };
	lTransform[3].pos = { 1100.0f,530.0f,0.0f };
	std::array<KazMath::Vec2<float>, 4> lScale;
	lScale[0] = { 2.0f,2.0f };
	lScale[1] = { 3.0f,2.0f };
	lScale[2] = { 2.5f,2.0f };
	lScale[3] = { 2.0f,2.6f };
	std::array<int, 4> lLogMax;
	lLogMax[0] = 11;
	lLogMax[1] = 11;
	lLogMax[2] = 11;
	lLogMax[3] = 14;

	if (isFirstFlag)
	{
		GameCommonData::Instance()->IsSecondFlag = true;
	}
	isFirstFlag = true;
	for (int i = 0; i < logWindow.size(); ++i)
	{
		logWindow[i].Init(lTransform[i], lScale[i], false, WINDOW_2D);
		log[i].Init(lTransform[i].pos.ConvertVec2() + KazMath::Vec2<float>(-128.0f / 2.0f + 10.0f, -128.0f / 2.0f + 25.0f) * lScale[i], false, lLogMax[i]);
		logWriteTimer[i] = 0;
		logWriteMaxTime[i] = KazMath::Rand(20, 10);
	}
	logAppearTimer = 0;
	logAppearMaxTime = KazMath::Rand(20, 10);
	logWindowIndex = 0;
	appearTimer = 0;
	gridTopRate = 0.0f;


	mainRenderTargetRender.data.handleData = mainRenderTarget->GetGameRenderTargetHandle();

	movie.Init("", 10);
	movie.Noise();

	pc.Init(renderTarget->GetGameRenderTargetHandle());

	KazMath::Transform3D lTrans;
	lTrans.pos = { 0.0f,-76.0f,80.0f };
	lTrans.scale = { 70.0f,70.0f,49.0f };
	lTrans.rotation.y = 180.0f;
	pc.SetTransform(lTrans);

	pc.BloomMonitor();

	initPlayerFlag = false;


	eyePos = { 0.0f,6.0f,20.0f };
	targetPos = { 0.0f,-29.0f,632.0f };
}

void TitleScene::Finalize()
{
	for (int i = 0; i < gridR.size(); ++i)
	{
		gridR[i]->Finalize();
	}
	for (int i = 0; i < gridR.size(); ++i)
	{
		mainGridR[i]->Finalize();
	}
}

void TitleScene::Input()
{
	if (KeyBoradInputManager::Instance()->InputTrigger(DIK_O))
	{
		sceneNum = 1;
	}
	KeyBoradInputManager *input = KeyBoradInputManager::Instance();

#pragma region カメラ操作
	debugCameraMove = { 0,0,0 };
	float debugSpeed = 100.0f;
	float debugSideSpeed = 1.0f;
	if (input->InputState(DIK_D))
	{
		debugCameraMove.x = -debugSpeed;
	}
	if (input->InputState(DIK_A))
	{
		debugCameraMove.x = debugSpeed;
	}
	if (input->InputState(DIK_W))
	{
		debugCameraMove.y = debugSpeed;
	}
	if (input->InputState(DIK_S))
	{
		debugCameraMove.y = -debugSpeed;
	}


	if (input->InputState(DIK_RIGHTARROW))
	{
		angle.x += debugSideSpeed;
	}
	if (input->InputState(DIK_LEFTARROW))
	{
		angle.x += -debugSideSpeed;
	}

	if (input->InputState(DIK_UPARROW))
	{
		angle.y += debugSideSpeed;
	}
	if (input->InputState(DIK_DOWNARROW))
	{
		angle.y += -debugSideSpeed;
	}

#pragma endregion


	selectMenu.Input(ControllerInputManager::Instance()->InputTrigger(XINPUT_GAMEPAD_DPAD_DOWN), ControllerInputManager::Instance()->InputTrigger(XINPUT_GAMEPAD_DPAD_UP));

	if (ControllerInputManager::Instance()->InputTrigger(XINPUT_GAMEPAD_A) && pressStartButtonFlag && !startGameFlag)
	{
		switch (selectMenu.GetNumber())
		{
		case 0:
			//ゲーム開始
			startGameFlag = true;
			break;

		case 1:
			//ゲーム終了
			endGameFlag = true;
			break;
		default:
			break;
		}
	}
	if (ControllerInputManager::Instance()->InputTrigger(XINPUT_GAMEPAD_A) && !pressStartButtonFlag)
	{
		pressStartButtonFlag = true;
		menuR[0].Init(basePos[0], "[play         ]");
		menuR[1].Init(basePos[1], "[exit         ]");
	}
}

void TitleScene::Update()
{
	//デバック用
	//eyePos = KazMath::CaluEyePosForDebug(eyePos, debugCameraMove, angle, 0.1f);
	//targetPos = KazMath::CaluTargetPosForDebug(eyePos, angle.x);
	CameraMgr::Instance()->Camera(eyePos, targetPos, { 0.0f,1.0f,0.0f }, 0);

	gridR[0]->Update(-1.0f);
	mainGridR[0]->Update(-1.0f, false);


	selectMenu.Update();
	for (int i = 0; i < menuR.size(); ++i)
	{
		menuR[i].Update(selectMenu.GetNumber());
	}


	//ImGui::Begin("UI");
	//KazImGuiHelper::InputTransform2D("TitleLogo", &titleLogoR.data.transform);
	//KazImGuiHelper::InputTransform2D("LineLogo", &lineLogoR.data.transform);
	//KazImGuiHelper::InputTransform2D("StartButton", &startButtonR.data.transform);
	//KazImGuiHelper::InputVec2("MenuStart", &basePos[0]);
	//KazImGuiHelper::InputVec2("MenuExit", &basePos[1]);
	//ImGui::End();

	/*ImGui::Begin("UI");
	KazImGuiHelper::InputVec3("EyePos", &eyePos);
	KazImGuiHelper::InputVec3("TargetPos", &targetPos);
	ImGui::End();*/

	++buttonTimer;
	if (20 <= buttonTimer)
	{
		buttonTimer = 0;
		buttonFlashFlag = !buttonFlashFlag;
	}

	pc.Update();
	movie.Update();

	//ゲーム開始時の演出ーーーーーー
	if (!startGameFlag)
	{
		return;
	}
	if (!initPlayerFlag)
	{
		player.Init({ 0.0f,0.0f,4.5f }, false, true);
		initPlayerFlag = true;
	}



	++appearTimer;
	//全体の終了
	bool lEndFlag = KazMath::ConvertSecondToFlame(5) <= appearTimer;

	//プレイヤーが一気に引く
	if (KazMath::ConvertSecondToFlame(1) - 30 <= appearTimer)
	{
		KazMath::Larp({ 0.0f,3.0f,-8.0f }, &eyePos, 0.1f);
		KazMath::Larp({ 0.0f,3.0f,0.0f }, &targetPos, 0.1f);
	}
	//ログウィンドウの起動
	if (KazMath::ConvertSecondToFlame(2) - 30 <= appearTimer)
	{
		++logAppearTimer;
		if (logAppearMaxTime <= logAppearTimer && logWindowIndex < logWindow.size())
		{
			logAppearMaxTime = KazMath::Rand(20, 10);
			logAppearTimer = 0;
			logWindow[logWindowIndex].Start();
			++logWindowIndex;
		}
	}
	//プレイヤーの登場
	if (KazMath::ConvertSecondToFlame(2) <= appearTimer)
	{
		for (int i = 0; i < logWindow.size(); ++i)
		{
			if (logWindow[i].IsEnd())
			{
				continue;
			}

			++logWriteTimer[i];
			if (logWriteMaxTime[i] <= logWriteTimer[i])
			{
				logWriteMaxTime[i] = KazMath::Rand(20, 10);
				log[i].WriteLog("WriteTestLog", 1.0f);
				logWriteTimer[i] = 0;
			}
		}



		for (int i = 0; i < playerAroundCircleR.size(); ++i)
		{
			playerAroundCircleR[i].data.transform.rotation.x += 2.0f;
			playerAroundCircleR[i].data.transform.rotation.y += 2.0f;
			playerAroundCircleR[i].data.transform.rotation.z += 2.0f;

			if (playerAroundCircleR[i].data.radius <= 5.0f && !lEndFlag)
			{
				playerAroundCircleR[i].data.radius += 0.1f;
			}
			else if (0.0f <= playerAroundCircleR[i].data.radius && lEndFlag)
			{
				playerAroundCircleR[i].data.radius += -0.1f;
			}
		}
		player.Update();
	}
	//背景グリッド現れる
	if (KazMath::ConvertSecondToFlame(3) <= appearTimer)
	{
		pc.SetMonitorTexture(movie.GetTexture());
		for (int i = 0; i < mainGridR.size(); ++i)
		{
			mainGridR[i]->Appear();
		}
	}
	//グリッドが動く
	if (KazMath::ConvertSecondToFlame(4) <= appearTimer)
	{
		movie.Stop();
		Rate(&gridTopRate, 0.01f, 1.0f);
		mainGridR[1]->Update(-200.0f + EasingMaker(Out, Cubic, gridTopRate) * 1000.0f, false);
	}

	//演出終了
	if (lEndFlag)
	{
		for (int i = 0; i < logWriteMaxTime.size(); ++i)
		{
			log[i].Finalize();
			logWindow[i].End();
		}
	}


	for (int i = 0; i < menuR.size(); ++i)
	{
		menuR[i].Finalize();
	}

	for (int i = 0; i < log.size(); ++i)
	{
		log[i].Update();
	}

	for (int i = 0; i < logWindow.size(); ++i)
	{
		logWindow[i].Update();
	}


	if (lEndFlag && playerAroundCircleR[0].data.radius <= 0.0f)
	{
		sceneNum = 1;
	}
}

void TitleScene::Draw()
{
	RenderTargetStatus::Instance()->SetDoubleBufferFlame();
	RenderTargetStatus::Instance()->ClearDoubuleBuffer(BG_COLOR);

	movie.Draw();

	//タイトル画面描画--------------------------------------------
	renderTarget->SetRenderTarget();
	for (int i = 0; i < gridR.size(); ++i)
	{
		gridR[i]->Draw();
	}


	if (!startGameFlag)
	{
		titleLogoR.Draw();
		lineLogoR.Draw();
		onlyControllerR.Draw();
	}
	if (buttonFlashFlag && !pressStartButtonFlag)
	{
		startButtonR.Draw();
	}
	else if (pressStartButtonFlag)
	{
		for (int i = 0; i < menuR.size(); ++i)
		{
			menuR[i].Draw();
		}
	}
	renderTarget->Draw();
	//タイトル画面描画--------------------------------------------


	mainRenderTarget->SetRenderTarget();

	if (startGameFlag)
	{
		for (int i = 0; i < log.size(); ++i)
		{
			log[i].Draw();
		}

		for (int i = 0; i < logWindow.size(); ++i)
		{
			logWindow[i].Draw();
		}
	}

	for (int i = 0; i < playerAroundCircleR.size(); ++i)
	{
		playerAroundCircleR[i].Draw();
	}

	if (KazMath::ConvertSecondToFlame(2) <= appearTimer)
	{
		player.Draw();
	}

	pc.Draw();
	for (int i = 0; i < mainGridR.size(); ++i)
	{
		mainGridR[i]->Draw();
	}

	mainRenderTarget->Draw();




	mainRenderTargetRender.Draw();
}

int TitleScene::SceneChange()
{
	if (sceneNum != -1)
	{
		int tmp = sceneNum;
		sceneNum = -1;
		return tmp;
	}

	return SCENE_NONE;
}