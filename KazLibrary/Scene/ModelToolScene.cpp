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
	m_camera.Update({}, {}, true);
	CameraMgr::Instance()->Camera(KazMath::Vec3<float>(-3.0f,0.0f,0.0f), KazMath::Vec3<float>(16.0f, 0.0f, 0.0f), {0.0f,1.0f,0.0f});

	m_modelTool.Update();
}

void ModelToolScene::Draw()
{
	m_modelTool.Draw(m_rasterizeRenderer);

	m_rasterizeRenderer.Sort();
	m_rasterizeRenderer.Render();
}

int ModelToolScene::SceneChange()
{
	return 0;
}
