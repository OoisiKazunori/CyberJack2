#pragma once
#include"../KazLibrary/Buffer/CreateGpuBuffer.h"
#include"../Game/Interface/IStage.h"
#include<array>
#include<vector>
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Event/LineFlashLight.h"
#include"../Game/Debug/ParameterMgr.h"
#include"../Game/Stage/DrawGrid.h"
#include"../Game/Stage/MountainObject.h"
#include"../Game/Stage/BlockMountain.h"
#include"../Game/Stage/WindowForAd.h"
#include"../Game/Effect/SearchLight.h"

/// <summary>
/// Rez + vaporwave風のステージ
/// </summary>
class RezStage :public IStage
{
public:
	RezStage();
	void Update()override;
	void Draw()override;

private:

	//グリッドの描画ーーー
	int timer;
	int maxTimer;
	//グリッドの描画ーーー


	//床にOBJ配置
	std::array<MountainObject, 80>floorObjectRender;

	float scaleRate;
	bool reversValueFlag;


	std::array<int, 50>filePassNum;
	ParameterMgr stageParamLoader;

	int selectNum;
	BoxPolygonRender selectingR;


	std::array<std::unique_ptr<DrawGrid>, 3> gridRender;

	std::unique_ptr<BoxPolygonRender> poly;
	RESOURCE_HANDLE lineDrawHandle;


	std::array<RESOURCE_HANDLE, 3> stageModelhandle;

	struct MatData
	{
		DirectX::XMFLOAT4 color;
		DirectX::XMMATRIX mat;
	};

	float appearRate;


	std::array<BlockMountain, 2>blockMountainArray;
	std::array<WindowForAd, 50> windowArray;
	std::array<SearchLight, 50> searchLightArray;
};