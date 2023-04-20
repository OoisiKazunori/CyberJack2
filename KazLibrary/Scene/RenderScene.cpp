#include "RenderScene.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

RenderScene::RenderScene()
{
	endGameFlag = false;

	DrawFunc::PipelineGenerateData lData;
	lData.desc = DrawFuncPipelineData::SetTest();
	lData.shaderDataArray.emplace_back(KazFilePathName::VertexShaderPath + "TestDraw.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
	lData.shaderDataArray.emplace_back(KazFilePathName::VertexShaderPath + "TestDraw.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);

	testRArray[0] = std::make_unique<DrawFunc::KazRender>(
		DrawFunc::SetDrawPolygonIndexData(&rasterizeRenderer, boxR.drawIndexInstanceCommandData, lData)
		);

	testRArray[1] = std::make_unique<DrawFunc::KazRender>(
		DrawFunc::SetDrawPolygonIndexData(&rasterizeRenderer, boxR.drawIndexInstanceCommandData, lData)
		);

	testRArray[2] = std::make_unique<DrawFunc::KazRender>(
		DrawFunc::SetDrawPolygonIndexData(&rasterizeRenderer, boxR.drawIndexInstanceCommandData, lData)
		);

	transformArray[0].pos = { 0.0f,0.0f,0.0f };
	transformArray[1].pos = { 10.0f,0.0f,0.0f };
	transformArray[2].pos = { 20.0f,0.0f,0.0f };

	colorArray[0] = { 155,155,155,255 };
	colorArray[1] = { 155,0,0,155 };
	colorArray[2] = { 0,155,0,55 };

	TextureResourceMgr::Instance()->LoadGraph("");

}

RenderScene::~RenderScene()
{
}

void RenderScene::Init()
{
	camera.Init({});
}

void RenderScene::PreInit()
{
}

void RenderScene::Finalize()
{
}

void RenderScene::Input()
{
}

void RenderScene::Update()
{
	camera.Update({}, {}, true);
	CameraMgr::Instance()->Camera(camera.GetEyePos(), camera.GetTargetPos(), { 0.0f,1.0f,0.0f });

	rasterizeRenderer.Update();
}

void RenderScene::Draw()
{
	RenderTargetStatus::Instance()->SetDoubleBufferFlame();
	RenderTargetStatus::Instance()->ClearDoubuleBuffer(BG_COLOR);

	for (int i = 0; i < testRArray.size(); ++i)
	{
		testRArray[i]->DrawCall(transformArray[i], colorArray[i], 0, motherMat);
	}

	rasterizeRenderer.Draw();

}

int RenderScene::SceneChange()
{
	return 0;
}
