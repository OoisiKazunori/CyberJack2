#pragma once
#include"../KazLibrary/Scene/SceneBase.h"
#include"../KazLibrary/RenderTarget/RenderTargetStatus.h"
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include"../KazLibrary/Render/BoxPolygonRender.h"
#include"../Game/Helper/CameraWork.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../Buffer/DescriptorHeapMgr.h"
#include"../Buffer/UavViewHandleMgr.h"
#include"../Buffer/Polygon.h"
#include"../KazLibrary/Helper/Compute.h"

#include"../KazLibrary/Loader/ObjResourceMgr.h"
#include"../KazLibrary/Loader/ModelLoader.h"

#include"Raytracing/BlasVector.h"
#include"Raytracing/Tlas.h"

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
	DispatchComputeShader compute;

	//std::array<std::unique_ptr<DrawFunc::KazRender>, 3> testRArray;
	std::array<KazMath::Transform3D, 5> transformArray;
	std::array<KazMath::Color, 3> colorArray;
	DirectX::XMMATRIX motherMat;

	KazBufferHelper::BufferData textureBuffer;

	KazMath::Vec2<int>texSize;

	Sprite3DRender spriteR;
	CameraWork camera;

	std::unique_ptr<GPUParticleRender> gpuParticleRender;

	bool texFlag;

	ResouceBufferHelper clearGBuffer;


	std::array<KazBufferHelper::BufferData, 2>gBuffer;
	KazBufferHelper::BufferData finalGBuffer;

	PolygonBuffer boxBuffer;
	PolygonBuffer boxNormalBuffer;
	PolygonBuffer texBuffer;
	PolygonIndexData boxData;
	PolygonIndexData planeData;
	PolygonIndexData boxNormalData;

	std::shared_ptr<ModelInfomation> model;

	DispatchComputeShader::DispatchData dispatchData;

	//レイトレにモデルを組み込む用の配列クラス
	Raytracing::BlasVector m_blasVector;
	//レイトレで描画するための情報
	Raytracing::Tlas m_tlas;


	//std::unique_ptr<DrawFunc::KazRender>normalGBufferRender;
	//std::unique_ptr<DrawFunc::KazRender>finalGBufferRender;

	DrawFuncData::DrawCallData drawSponza;

	struct DrawGBufferData
	{
		std::string m_bufferName;
		bool m_drawFlag;
		DrawFuncData::DrawCallData m_plane;
	};
	std::array<DrawGBufferData, 4>m_drawPlaneArray;
	DrawGBufferData m_drawFinalPlane;

	KazMath::Vec3<float>lightVec;
};

