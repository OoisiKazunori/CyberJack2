#pragma once
#include"../KazLibrary/Scene/SceneBase.h"
#include"../KazLibrary/RenderTarget/RenderTargetStatus.h"
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include"../KazLibrary/Render/BoxPolygonRender.h"
#include"../Game/Helper/CameraWork.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../Buffer/DescriptorHeapMgr.h"
#include"../Buffer/UavViewHandleMgr.h"


class RenderScene :public SceneBase
{
public:
	RenderScene();
	~RenderScene();

	void Init();
	void PreInit();
	void Finalize();
	void Input();
	void Update();
	void Draw();

	int SceneChange();

private:
	DrawingByRasterize rasterizeRenderer;

	std::array<std::unique_ptr<DrawFunc::KazRender>, 3> testRArray;
	std::array<KazMath::Transform3D, 3> transformArray;
	std::array<KazMath::Color, 3> colorArray;
	DirectX::XMMATRIX motherMat;

	std::shared_ptr<KazBufferHelper::BufferData>textureBuffer;

	KazMath::Vec2<int>texSize;

	BoxPolygonRender boxR;
	Sprite3DRender spriteR;
	CameraWork camera;

	std::unique_ptr<GPUParticleRender> gpuParticleRender;

};

