#include "ModelToolScene.h"

ModelToolScene::ModelToolScene() :m_modelTool("Resource/Model/")
{
}

ModelToolScene::~ModelToolScene()
{
}

void ModelToolScene::Init()
{
	CameraMgr::Instance()->CameraSetting(60.0f, 20000.0f);
}

void ModelToolScene::PreInit()
{
}

void ModelToolScene::Finalize()
{
}

void ModelToolScene::Input()
{
}

void ModelToolScene::Update()
{
	m_debugCamera.Update();
	m_modelTool.Update();
}

void ModelToolScene::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{
	m_modelTool.Draw(arg_rasterize);
}

int ModelToolScene::SceneChange()
{
	return 0;
}
