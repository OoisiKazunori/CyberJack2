#pragma once
#include"../KazLibrary/Scene/SceneBase.h"
#include"../KazLibrary/RenderTarget/RenderTargetStatus.h"

class RayTracingScene :public SceneBase
{
public:
	RayTracingScene();
	~RayTracingScene();

	void Init();
	void PreInit();
	void Finalize();
	void Input();
	void Update();
	void Draw();

	int SceneChange();
};

