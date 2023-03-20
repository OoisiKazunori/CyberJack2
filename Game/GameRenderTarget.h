#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../KazLibrary/RenderTarget/GaussianBuler.h"
#include"../KazLibrary/Math/KazMath.h"

class GameRenderTarget
{
public:
	GameRenderTarget(const KazMath::Color &COLOR);
	~GameRenderTarget();
	void Init();
	void Finalize();
	void Update();

	void SetRenderTarget();
	void Draw();

	RESOURCE_HANDLE GetGameRenderTargetHandle();

private:
	Sprite2DRender mainRenderTarget;
	Sprite2DRender addRenderTarget;
	Sprite2DRender luminaceTex;

	std::vector<RESOURCE_HANDLE> handles;
	RESOURCE_HANDLE addHandle;
	RESOURCE_HANDLE gameTexHandle;
	std::array<std::unique_ptr<GaussianBuler>, 4> buler;
};