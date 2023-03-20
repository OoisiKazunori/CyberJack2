#include "RezStage.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Easing/easing.h"
#include"../KazLibrary/Helper/KazHelper.h"

RezStage::RezStage()
{
	poly = std::make_unique<BoxPolygonRender>(true, 400);
	poly->data.pipelineName = PIPELINE_NAME_INSTANCE_COLOR;
	lineDrawHandle = poly->CreateConstBuffer(sizeof(MatData) * 500, "MatData", GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DRAW);


	for (int i = 0; i < gridRender.size(); ++i)
	{
		gridRender[i] = std::make_unique<DrawGrid>(KazMath::Color(29, 19, 72, 255));
	}

	KazMath::Color lBaseColor(115 - 50, 85 - 50, 140 - 50, 255);
	KazMath::Color lFlashColor(213, 5, 228, 255);
	std::array<KazMath::Color, 2>lFlashColorArray = { lBaseColor,lFlashColor };


	gridRender[0]->Init(true, 300.0f, -150.0f, &cameraIndex, true, true, lFlashColorArray);
	//gridRender[1]->Init(true, 300.0f, 2000.0f, &cameraIndex, true, true, lFlashColorArray);
	gridRender[1]->Init(false, 300.0f, -3000.0f, &cameraIndex, true, false, lFlashColorArray);
	gridRender[2]->Init(false, 300.0f, 3000.0f, &cameraIndex, true, false, lFlashColorArray);


	for (int i = 0; i < filePassNum.size(); ++i)
	{
		filePassNum[i] = 2;
	}

	stageModelhandle[0] = ObjResourceMgr::Instance()->LoadModel(KazFilePathName::StagePath + "Mountain01_Model.obj");
	stageModelhandle[1] = ObjResourceMgr::Instance()->LoadModel(KazFilePathName::StagePath + "Mountain02_Model.obj");
	stageModelhandle[2] = ObjResourceMgr::Instance()->LoadModel(KazFilePathName::StagePath + "Mountain03_Model.obj");

	vaporWaveSunRender.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::RelativeResourcePath + "Stage/" + "vaporWaveSun.png");
	vaporWaveSunRender.data.transform.pos = { 0.0f,600.0f,5000.0f };
	vaporWaveSunRender.data.transform.scale = { 0.0f,0.0f,1.0f };
	vaporWaveSunRender.data.colorData = { 255,0,0,255 };
	vaporWaveSunRender.data.pipelineName = PIPELINE_NAME_SPRITE_MULTITEX;

	maxTimer = 60;



	selectingR.data.pipelineName = PIPELINE_NAME_COLOR_WIREFLAME;
	selectingR.data.color = { 255,0,0,255 };



	stageParamLoader.LoadFile(KazFilePathName::StageParamPath + "RezStageParamData.json");
	//ÉpÉâÉÅÅ[É^Ç©ÇÁîªíf
	for (int i = 0; i < 40; ++i)
	{
		std::string name = "FloorObj" + std::to_string(i);
		KazMath::Transform3D initTrans;
		initTrans.pos.x = stageParamLoader.doc[name.c_str()]["Pos"][0].GetFloat();
		initTrans.pos.y = stageParamLoader.doc[name.c_str()]["Pos"][1].GetFloat();
		initTrans.pos.z = stageParamLoader.doc[name.c_str()]["Pos"][2].GetFloat();

		initTrans.scale.x = stageParamLoader.doc[name.c_str()]["Scale"][0].GetFloat();
		initTrans.scale.y = stageParamLoader.doc[name.c_str()]["Scale"][1].GetFloat();
		initTrans.scale.z = stageParamLoader.doc[name.c_str()]["Scale"][2].GetFloat();

		initTrans.rotation.x = stageParamLoader.doc[name.c_str()]["Rota"][0].GetFloat();
		initTrans.rotation.y = stageParamLoader.doc[name.c_str()]["Rota"][1].GetFloat();
		initTrans.rotation.z = stageParamLoader.doc[name.c_str()]["Rota"][2].GetFloat();

		int lNum = stageParamLoader.doc[name.c_str()]["FilePass"].GetInt();
		floorObjectRender[i].Init(initTrans, stageModelhandle[lNum], &cameraIndex);

		initTrans.pos.z -= 9000.0f;
		floorObjectRender[40 + i].Init(initTrans, stageModelhandle[lNum], &cameraIndex);
	}
	appearRate = 0.0f;



	InitBlockMountainData lInitData;
	lInitData.fogDesinty = 0.0003f;
	lInitData.lightDir = { -1.0f,1.0f,1.0f };
	lInitData.xRange = 70;
	lInitData.yScale = 500.0f;
	lInitData.centralPos = { 0.0f,-2000.0f,0.0f };
	lInitData.rotation = { 0.0f,0.0f,0.0f };
	blockMountainArray[0].Init(lInitData);

	lInitData.fogDesinty = 0.0005f;
	lInitData.lightDir = { 0.0f,0.0f,1.0f };
	lInitData.xRange = 70;
	lInitData.yScale = 500.0f;
	lInitData.centralPos = { 0.0f,2500.0f,0.0f };
	lInitData.rotation = { 180.0f,0.0f,0.0f };
	blockMountainArray[1].Init(lInitData);

	std::array<RESOURCE_HANDLE, 10>lData;
	for (int i = 0; i < lData.size(); ++i)
	{
		std::string lFilePass = "WindowTex" + std::to_string(i) + ".png";
		lData[i] = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::StagePath + "WindowTex/" + lFilePass);
	}


	for (int i = 0; i < windowArray.size(); ++i)
	{
		float lX = KazMath::Rand(5000.0f, 1000.0f);
		if (2 <= KazMath::Rand(4, 0))
		{
			lX = KazMath::Rand(5000.0f, 1500.0f);
		}
		else
		{
			lX = -KazMath::Rand(5000.0f, 1500.0f);
		}

		KazMath::Vec3<float>pos(lX, KazMath::Rand(500.0f, -500.0f), KazMath::Rand(10000.0f, -10000.0f));
		windowArray[i].Init(pos, lData);
	}

	for (int i = 0; i < searchLightArray.size(); ++i)
	{
		KazMath::Vec3<float>lPos = { 0.0f,-1450.0f, KazMath::Rand(10000.0f, -1000.0f) };
		if (1 <= KazMath::Rand(2, 0))
		{
			lPos.x = KazMath::Rand(10000.0f, 500.0f);
		}
		else
		{
			lPos.x = -KazMath::Rand(10000.0f, 500.0f);
		}
		searchLightArray[i].Init(lPos, KazMath::Color(KazMath::Rand(255, 100), KazMath::Rand(255, 100), KazMath::Rand(255, 100), 255));
	}
}

void RezStage::Update()
{
	for (int i = 0; i < gridRender.size(); ++i)
	{
		gridRender[i]->Update(-1.0f, false);
	}

	for (int i = 0; i < floorObjectRender.size(); ++i)
	{
		floorObjectRender[i].Update();
	}

	if (startFlag)
	{
		Rate(&appearRate, 0.01f, 1.0f);
		float easeRate = EasingMaker(Out, Cubic, appearRate) * 2.0f;
		vaporWaveSunRender.data.transform.scale.x = easeRate;
		vaporWaveSunRender.data.transform.scale.y = easeRate;
		vaporWaveSunRender.data.transform.scale.z = easeRate;
	}

	for (int i = 0; i < blockMountainArray.size(); ++i)
	{
		blockMountainArray[i].Update();
	}

	for (int i = 0; i < windowArray.size(); ++i)
	{
		windowArray[i].Update();
	}

	for (int i = 0; i < searchLightArray.size(); ++i)
	{
		searchLightArray[i].Update();
	}
}

void RezStage::Draw()
{
	for (int i = 0; i < floorObjectRender.size(); ++i)
	{
		floorObjectRender[i].Draw();
	}

	for (int i = 0; i < gridRender.size(); ++i)
	{
		gridRender[i]->Draw();
	}

	for (int i = 0; i < blockMountainArray.size(); ++i)
	{
		blockMountainArray[i].Draw();
	}

	for (int i = 0; i < windowArray.size(); ++i)
	{
		windowArray[i].Draw();
	}

	for (int i = 0; i < searchLightArray.size(); ++i)
	{
		searchLightArray[i].Draw();
	}
}