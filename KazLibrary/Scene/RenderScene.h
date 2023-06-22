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
#include"Raytracing/HitGroupMgr.h"
#include"Raytracing/RayPipeline.h"

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
	DrawingByRasterize m_rasterizeRenderer;
	DispatchComputeShader m_compute;

	//std::array<std::unique_ptr<DrawFunc::KazRender>, 3> testRArray;
	std::array<KazMath::Transform3D, 5> m_transformArray;
	std::array<KazMath::Color, 3> m_colorArray;
	DirectX::XMMATRIX m_motherMat;

	KazBufferHelper::BufferData m_textureBuffer;

	KazMath::Vec2<int>m_texSize;

	Sprite3DRender m_spriteR;
	CameraWork m_camera;

	std::unique_ptr<GPUParticleRender> m_gpuParticleRender;

	bool m_texFlag;

	ResouceBufferHelper m_clearGBuffer;


	std::array<KazBufferHelper::BufferData, 2>m_gBuffer;
	KazBufferHelper::BufferData m_finalGBuffer;

	PolygonBuffer m_boxBuffer;
	PolygonBuffer m_bboxNormalBuffer;
	PolygonBuffer m_texBuffer;
	PolygonIndexData m_boxData;
	PolygonIndexData m_planeData;
	PolygonIndexData m_pboxNormalData;

	std::shared_ptr<ModelInfomation> m_sponzaModel;
	std::shared_ptr<ModelInfomation> m_suzanneModel;

	DispatchComputeShader::DispatchData m_dispatchData;

	//レイトレにモデルを組み込む用の配列クラス
	Raytracing::BlasVector m_blasVector;
	//レイトレで描画するための情報
	Raytracing::Tlas m_tlas;
	//レイトレ用パイプライン
	std::vector<Raytracing::RayPipelineShaderData> m_pipelineShaders;
	std::unique_ptr<Raytracing::RayPipeline> m_rayPipeline;


	//std::unique_ptr<DrawFunc::KazRender>normalGBufferRender;
	//std::unique_ptr<DrawFunc::KazRender>finalGBufferRender;

	DrawFuncData::DrawCallData m_drawSponza;
	DrawFuncData::DrawCallData m_drawSuzanne;

	struct DrawGBufferData
	{
		std::string m_bufferName;
		bool m_drawFlag;
		DrawFuncData::DrawCallData m_plane;
	};
	std::array<DrawGBufferData, 4>m_drawPlaneArray;
	DrawGBufferData m_drawFinalPlane;

	KazMath::Vec3<float>m_lightVec;
	KazMath::Vec3<float>m_atem;

	struct LightData
	{
		DirectX::XMFLOAT3 pos;
		float pad;
		DirectX::XMFLOAT3 atem;
	};
};

