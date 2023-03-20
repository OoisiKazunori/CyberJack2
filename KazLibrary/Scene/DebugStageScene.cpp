#include "DebugStageScene.h"
#include"../Camera/CameraMgr.h"
#include"../Input/KeyBoradInputManager.h"
#include"../Loader/FbxModelResourceMgr.h"
#include"../Helper/ResourceFilePass.h"
#include"../RenderTarget/RenderTargetStatus.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../Game/Stage/BlockParticleStage.h"
#include"../Game/Stage/FirstStage.h"
#include"../Game/Stage/RezStage.h"


#include"../KazLibrary/Input/ControllerInputManager.h"

DebugStageScene::DebugStageScene()
{
	stages[0] = std::make_shared<FirstStage>();
	stages[1] = std::make_shared<RezStage>();
	stages[2] = std::make_shared<BlockParticleStage>();


	renderTarget[0] = std::make_unique<GameRenderTarget>(KazMath::Color(29, 19, 72, 255));
	renderTarget[1] = std::make_unique<GameRenderTarget>(KazMath::Color(29, 19, 72, 255));
	renderTarget[2] = std::make_unique<GameRenderTarget>(KazMath::Color(0, 0, 0, 255));


	mainRenderTarget.data.transform.pos = { WIN_X / 2.0f,WIN_Y / 2.0f };

	for (int i = 0; i < stages.size(); ++i)
	{
		if (stages[i] != nullptr)
		{
			stages[i]->startFlag = true;
		}
	}

	stageNum = 2;

	//stage2 = std::make_shared<BlockParticleStage>();
}

DebugStageScene::~DebugStageScene()
{
}

void DebugStageScene::Init()
{
	player.Init({ 0.0f,0.0f,15.0f }, false, false);
	cameraWork.Init();
	cursor.Init();
	//mapchipTool.Init();

	gameCameraFlag = true;


	goalBox.Init({ 0.0f, 10.0f, 40.0f });
	goalBox.Appear({ 20.0f, 10.0f, 100.0f });


	for (int i = 0; i < 8; ++i)
	{
		goalBox.lightEffect.Appear();
	}
}

void DebugStageScene::Finalize()
{
}

void DebugStageScene::Input()
{
#ifdef _DEBUG
#else
#endif
	KeyBoradInputManager *input = KeyBoradInputManager::Instance();

	ControllerInputManager *inputController = ControllerInputManager::Instance();

	bool upFlag = false;
	bool downFlag = false;
	bool leftFlag = false;
	bool rightFlag = false;
	bool doneFlag = false;
	bool releaseFlag = false;

	const int DEAD_ZONE = 3000;
	if (inputController->InputState(XINPUT_GAMEPAD_A))
	{
		doneFlag = true;
	}
	if (inputController->InputRelease(XINPUT_GAMEPAD_A))
	{
		releaseFlag = true;
	}
	if (inputController->InputStickState(LEFT_STICK, UP_SIDE, DEAD_ZONE))
	{
		upFlag = true;
	}
	if (inputController->InputStickState(LEFT_STICK, DOWN_SIDE, DEAD_ZONE))
	{
		downFlag = true;
	}
	if (inputController->InputStickState(LEFT_STICK, LEFT_SIDE, DEAD_ZONE))
	{
		leftFlag = true;
	}
	if (inputController->InputStickState(LEFT_STICK, RIGHT_SIDE, DEAD_ZONE))
	{
		rightFlag = true;
	}

	KazMath::Vec2<float> joyStick;
	joyStick.x = inputController->GetJoyStickLXNum(0) / 32767.0f;
	joyStick.y = inputController->GetJoyStickLYNum(0) / 32767.0f;

	cursor.Input
	(
		upFlag,
		downFlag,
		leftFlag,
		rightFlag,
		doneFlag,
		releaseFlag,
		joyStick
	);


	ImGui::Begin("Stage");
	ImGui::InputInt("StageNum", &stageNum);
	ImGui::Checkbox("ToolMode", &toolModeFlag);
	ImGui::Checkbox("CameraModeFlag", &gameCameraFlag);
	ImGui::InputInt("CameraDir", &dir);
	ImGui::End();

	cameraWork.ForceCamera(static_cast<KazEnemyHelper::CameraDir>(dir));

	if (stageNum <= 0)
	{
		stageNum = 0;
	}
	if (stages.size() <= stageNum)
	{
		stageNum = static_cast<int>(stages.size() - 1);
	}

	for (int i = 0; i < stages.size(); ++i)
	{
		if (stages[i] != nullptr)
		{
			stages[i]->startFlag = true;
		}
	}

	if (toolModeFlag)
	{
		mapchipTool.Input(input->MouseInputTrigger(MOUSE_INPUT_LEFT), input->MouseInputTrigger(MOUSE_INPUT_RIGHT), input->GetMousePoint());
	}
#ifdef _DEBUG
#else
#endif
}

void DebugStageScene::Update()
{
	eyePos = cameraWork.GetEyePos();
	targetPos = cameraWork.GetTargetPos();
	CameraMgr::Instance()->CameraSetting(60.0f, 100000.0f, 0);
	CameraMgr::Instance()->Camera(eyePos, targetPos, { 0.0f,1.0f,0.0f });

	player.Update();
	cursor.Update();


	cameraWork.Update(cursor.GetValue(), &player.pos, gameCameraFlag);
	if (toolModeFlag)
	{
		mapchipTool.Update();
		if (mapchipTool.isLoadFlag)
		{
			isLoadFlag = true;
		}
#ifdef _DEBUG
#else
#endif
	}
	else
	{
		if (isLoadFlag)
		{
			mapchipTool.Init();
			stages[stageNum].reset();
			stages[stageNum] = std::make_unique<BlockParticleStage>();
			isLoadFlag = false;
		}
#ifdef _DEBUG
#else
#endif
		stages[stageNum]->Update();
		//stage2->Update();
	}


	goalBox.Update();


	mainRenderTarget.data.handleData = renderTarget[stageNum]->GetGameRenderTargetHandle();
}

void DebugStageScene::Draw()
{
	RenderTargetStatus::Instance()->SetDoubleBufferFlame();
	RenderTargetStatus::Instance()->ClearDoubuleBuffer(BG_COLOR);
	renderTarget[stageNum]->SetRenderTarget();
	player.Draw();

	if (toolModeFlag)
	{
		mapchipTool.Draw();
	}
	else
	{
		stages[stageNum]->Draw();
		stages[stageNum]->vaporWaveSunRender.Draw();
	}

	//goalBox.Draw();
	//goalBox.lightEffect.Draw();

	renderTarget[stageNum]->Draw();
	mainRenderTarget.Draw();

	if (!toolModeFlag)
	{
		//stage2->Draw();
		cursor.Draw();
	}
}

int DebugStageScene::SceneChange()
{
	return SCENE_NONE;
}
