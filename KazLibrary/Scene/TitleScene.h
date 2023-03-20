#pragma once
#include"../DirectXCommon/Base.h"
#include"../Scene/SceneBase.h"
#include"../Game/Stage/DrawGrid.h"
#include"../KazLibrary/RenderTarget/GaussianBuler.h"
#include"../KazLibrary/Helper/Select.h"
#include"../Game/UI/DrawMenu.h"
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Player.h"
#include"../Game/UI/AttackLog.h"
#include"../Game/Event/WindowFlame.h"
#include"../Game/GameRenderTarget.h"

#include"../Game/Tutorial/OldComputer.h"
#include"../Game/Tutorial/TutorialMovie.h"

class TitleScene :public SceneBase {

public:
	TitleScene();
	~TitleScene();

	void Init();
	void Finalize();
	void Input();
	void Update();
	void Draw();

	int SceneChange();

private:
	int sceneNum;

	int cameraIndex;
	KazMath::Vec3<float> debugCameraMove;
	KazMath::Vec3<float> eyePos, targetPos;
	KazMath::Vec2<float> angle;

	std::array<std::unique_ptr<DrawGrid>, 2> gridR;
	std::array<std::unique_ptr<DrawGrid>, 2> mainGridR;

	std::unique_ptr<GameRenderTarget>renderTarget;
	std::unique_ptr<GameRenderTarget>mainRenderTarget;
	Sprite2DRender mainRenderTargetRender;



	Sprite2DRender titleLogoR;
	Sprite2DRender lineLogoR;
	Sprite2DRender startButtonR;
	int buttonTimer;
	bool buttonFlashFlag;
	bool pressStartButtonFlag;

	static const int MENU_MAX_NUM = 2;
	Select selectMenu;
	std::array<MenuRender, MENU_MAX_NUM> menuR;
	std::array<KazMath::Vec2<float>, MENU_MAX_NUM> basePos;

	bool startGameFlag;
	Player player;
	bool initPlayerFlag;
	std::array<CircleRender, 2> playerAroundCircleR;

	std::array<AttackLog, 4> log;
	std::array<WindowFlame, 4>logWindow;
	int logAppearTimer, logAppearMaxTime;
	int logWindowIndex;
	std::array<int, 4> logWriteTimer;
	std::array<int, 4> logWriteMaxTime;

	int appearTimer;
	float gridTopRate;


	OldComputer pc;
	TutorialMovie movie;

	Sprite2DRender onlyControllerR;
};