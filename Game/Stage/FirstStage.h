#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Debug/ParameterMgr.h"
#include"../Game/Interface/IStage.h"
#include<array>
#include<vector>


struct FogD
{
	//フォグカラー
	DirectX::XMFLOAT3 endColor;
	float pad;
	DirectX::XMFLOAT3 subValue;
	float pad2;
	//x...RataMin,y...RateMax,z...StartFogDepth,w...EndFogDepth
	DirectX::XMFLOAT4 rateAndFogLine;
	DirectX::XMFLOAT3 depthX;
	float pad3;
};

class FirstStage :public IStage
{
public:
	FirstStage();
	void Update()override;
	void Draw()override;

	KazMath::Color firstColor;
	KazMath::Color endColor;

	float easeY;
	float t;

	std::array<ObjModelRender, 6> floorStage;
	std::array<ObjModelRender, 12> pillarObj;
	std::array<ObjModelRender, 12> farSidePillarObj;
	std::array<std::array<ObjModelRender, 12>, 3> topPillarObj;
	std::array<std::array<float, 12>, 3> initTopPillarPosX;


	void InitPillar(ObjModelRender *MODEL)
	{
		MODEL->data.handle = ObjResourceMgr::Instance()->LoadModel(KazFilePathName::StagePath + "house/" + "house.obj");
		MODEL->data.pipelineName = PIPELINE_NAME_OBJ_FOG_GRADATION;
		RESOURCE_HANDLE lHandle = MODEL->CreateConstBuffer(sizeof(FogD), typeid(FogD).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA2);

		FogD lD;
		lD.rateAndFogLine.z = 100.0f;
		lD.rateAndFogLine.w = 300.0f;

		lD.rateAndFogLine.x = -401.0f;
		lD.rateAndFogLine.y = 407.0f;
		lD.depthX.z = 0;

		DirectX::XMFLOAT3 first(firstColor.ConvertColorRateToXMFLOAT3());
		DirectX::XMFLOAT3 end(endColor.ConvertColorRateToXMFLOAT3());
		DirectX::XMFLOAT3 result;
		result.x = first.x - end.x;
		result.y = first.y - end.y;
		result.z = first.z - end.z;
		lD.endColor = end;
		lD.subValue = result;

		MODEL->TransData(&lD, lHandle, typeid(lD).name());
	};
};

