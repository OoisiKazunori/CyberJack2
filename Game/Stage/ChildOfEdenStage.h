#pragma once
#include"../Game/Interface/IStage.h"
#include<array>
#include<vector>
#include"../KazLibrary/Buffer/DrawFuncData.h"
#include"../KazLibrary/Render/DrawFunc.h"
#include"../KazLibrary/Loader/ModelLoader.h"

class ChildOfEdenStage :public IStage
{
public:
	ChildOfEdenStage();
	void Update()override;
	void Draw(DrawingByRasterize& arg_rasterize)override;

public:
	KazMath::Transform3D m_skydormTransform;
	float m_skydormScale;
	DrawFuncData::DrawCallData m_drawSkydorm;
};

