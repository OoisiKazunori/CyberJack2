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
#include"../KazLibrary/Render/DrawFunc.h"

#include"../Render/CPUParticle.h"

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
	void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec);

	int SceneChange();

private:

	//std::array<std::unique_ptr<DrawFunc::KazRender>, 3> testRArray;
	std::array<KazMath::Transform3D, 5> m_transformArray;
	std::array<KazMath::Color, 3> m_colorArray;
	DirectX::XMMATRIX m_motherMat;

	KazBufferHelper::BufferData m_textureBuffer;

	KazMath::Vec2<int>m_texSize;

	Sprite3DRender m_spriteR;
	CameraWork m_camera;

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

	std::shared_ptr<ModelInfomation> m_model;
	std::shared_ptr<ModelInfomation> m_reflectionModel;
	std::shared_ptr<ModelInfomation> m_refractionModel;

	KazBufferHelper::BufferData m_mipmapTexture;

	//テスト用モデル
	std::array<std::shared_ptr<ModelInfomation>, 6>m_testModelArray;

	struct TestModelField
	{
		std::array<std::array<DrawFuncData::DrawCallData, 6>, 6>m_modelArray;	//モデル
		std::array<std::array<KazMath::Transform3D, 6>, 6>m_transformArray;		//Transform

		TestModelField(std::array<std::shared_ptr<ModelInfomation>, 6> arg_modelArray, std::array<float, 6>arg_float)
		{
			for (int y = 0; y < m_modelArray.size(); ++y)
			{
				for (int x = 0; x < m_modelArray[y].size(); ++x)
				{
					m_modelArray[y][x] = DrawFuncData::SetDefferdRenderingModel(arg_modelArray[y]);
					m_transformArray[y][x].scale = { arg_float[y],arg_float[y],arg_float[y] };
					m_transformArray[y][x].rotation.y = 90.0f;

					KazMath::Color color(255, 255, 255, 255);
					m_modelArray[y][x].extraBufferArray.back().bufferWrapper->TransData(&color.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));
				}
			}
		};

		void SetPos(std::array<float, 6>arg_baseX, float arg_interval, float arg_height)
		{
			for (int y = 0; y < m_modelArray.size(); ++y)
			{
				for (int x = 0; x < m_modelArray[y].size(); ++x)
				{
					float offset = static_cast<float>(x) * arg_interval;
					m_transformArray[y][x].pos = { -500.0f + offset,arg_height,arg_baseX[y] };
				}
			}
		};

		void Update(DrawingByRasterize& arg_drawCall, Raytracing::BlasVector& arg_blasVec)
		{
			for (int y = 0; y < m_modelArray.size(); ++y)
			{
				for (int x = 0; x < m_modelArray[y].size(); ++x)
				{
					DrawFunc::DrawModelInRaytracing(m_modelArray[y][x], m_transformArray[y][x], DrawFunc::NONE);
					arg_drawCall.ObjectRender(m_modelArray[y][x]);

					for (auto& blas : m_modelArray[y][x].m_raytracingData.m_blas)
					{
						arg_blasVec.Add(blas, m_transformArray[y][x].GetMat());
					}
				}
			}
		};
	};
	std::vector<TestModelField>m_testModelFiledArray;

	bool m_lightFlag;
	static const int LGHIT_ARRAY_X = 5;
	static const int LGHIT_ARRAY_Y = 8;
	static const int LGHIT_ARRAY_Z = 5;
	std::array<std::array<std::array<DrawFuncData::DrawCallData, LGHIT_ARRAY_Z>, LGHIT_ARRAY_X>, LGHIT_ARRAY_Y> m_lightBoxDataArray;
	std::array<std::array<std::array<KazMath::Transform3D, LGHIT_ARRAY_Z>, LGHIT_ARRAY_X>, LGHIT_ARRAY_Y> m_lightTransformArray;
	KazBufferHelper::BufferData lightUploadBuffer;

	ComputeShader m_dispatch;


	DrawFuncData::DrawCallData m_alphaModel;
	KazMath::Transform3D m_alphaModelTransform;



	//std::unique_ptr<DrawFunc::KazRender>normalGBufferRender;
	//std::unique_ptr<DrawFunc::KazRender>finalGBufferRender;

	DrawFuncData::DrawCallData m_drawSponza;
	DrawFuncData::DrawCallData m_reflectionSphere;
	DrawFuncData::DrawCallData m_refractionSphere;
	KazMath::Transform3D m_sphereTransform;

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

	CPUParticle m_particle;
};

