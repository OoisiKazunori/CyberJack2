#pragma once
#include"../Scene/SceneBase.h"
#include"../Render/BackGroundForDebug.h"
#include"../Render/KazRender.h"
#include"../Game/Player.h"
#include"../Game/UI/Cursor.h"
#include"../Game/Helper/CameraWork.h"
#include"../Game/Effect/PortalIntermediateDirection.h"
#include"../Game/GameRenderTarget.h"
#include"../Game/Interface/IStage.h"
#include"../Game/Tool/MapChipPlacementTool.h"
#include"../Game/Event/GoalBox.h"

class DebugStageScene :public SceneBase
{
public:
	DebugStageScene();
	~DebugStageScene();

	void Init();
	void Finalize();
	void Input();
	void Update();
	void Draw();

	int SceneChange();

private:
	KazMath::Vec3<float> debugCameraMove;
	KazMath::Vec3<float> eyePos, targetPos;
	KazMath::Vec2<float> angle;


	std::vector<RESOURCE_HANDLE>multiHandle;

	Player player;
	Cursor cursor;
	CameraWork cameraWork;
	int stageNum;
	std::array<std::shared_ptr<IStage>, 3>stages;
	std::shared_ptr<IStage> stage2;
	std::array<std::unique_ptr<GameRenderTarget>, 3> renderTarget;

	Sprite2DRender mainRenderTarget;

#ifdef _DEBUG
#else
#endif
	MapChipPlacementTool mapchipTool;


	bool toolModeFlag;
	bool isLoadFlag;

	bool gameCameraFlag;
	int dir;
	GoalBox goalBox;
};

