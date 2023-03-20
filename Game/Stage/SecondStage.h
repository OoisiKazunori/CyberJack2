#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../Game/Interface/IStage.h"
#include"../Game/Debug/ParameterMgr.h"
#include"../KazLibrary/Render/KazRender.h"
#include<array>

struct FogHandle
{
	UINT32 handle;
	FogData fogColor;
};

class SecondStage :public IStage
{
public:
	SecondStage();
	void Update()override;
	void Draw()override;
private:
	static const int JSON_ARRAY_NUM = 25;
	std::array<LineRender, 150> line;
	std::array<FogHandle, 150> fogHandle;
	ParameterMgr stageParamLoader;

};

