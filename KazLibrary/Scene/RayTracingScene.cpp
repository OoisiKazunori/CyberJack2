#include "RayTracingScene.h"

RayTracingScene::RayTracingScene()
{
	endGameFlag = false;
}

RayTracingScene::~RayTracingScene()
{
}

void RayTracingScene::Init()
{
}

void RayTracingScene::PreInit()
{
}

void RayTracingScene::Finalize()
{
}

void RayTracingScene::Input()
{
}

void RayTracingScene::Update()
{
}

void RayTracingScene::Draw()
{
	RenderTargetStatus::Instance()->SetDoubleBufferFlame();
	RenderTargetStatus::Instance()->ClearDoubuleBuffer(BG_COLOR);
}

int RayTracingScene::SceneChange()
{
	return 0;
}
