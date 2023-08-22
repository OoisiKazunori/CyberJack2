#pragma once
#include"../Game/Interface/IStage.h"
#include<array>
#include<vector>
#include"../KazLibrary/Buffer/DrawFuncData.h"
#include"../KazLibrary/Render/DrawFunc.h"
#include"../KazLibrary/Loader/ModelLoader.h"
#include"../KazLibrary/Helper/Compute.h"

class ChildOfEdenStage :public IStage
{
public:
	ChildOfEdenStage();
	void Update()override;
	void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)override;

public:
	KazMath::Transform3D m_skydormTransform;
	float m_skydormScale;
	//DrawFuncData::DrawCallData m_drawSkydorm;

	//ステージ内に漂う三角パーティクル
	DrawFuncData::DrawCallData m_drawTriangleParticle;
	//上記のパーティクルをレイトレで描画する
	DrawFuncData::DrawCallData m_drawTriangleParticleInRaytracing;

	static const int DISPATCH_MAX_NUM = 1;
	static const int PARTICLE_MAX_NUM = 1024 * DISPATCH_MAX_NUM;
	struct ParticeArgumentData
	{
		DirectX::XMFLOAT3 m_pos;
		DirectX::XMFLOAT3 m_scale;
		DirectX::XMFLOAT3 m_rotation;
		DirectX::XMFLOAT3 m_rotationVel;
		DirectX::XMFLOAT4 m_color;
	};
	struct CameraBufferData
	{
		DirectX::XMMATRIX m_billboardMat;
		DirectX::XMMATRIX m_viewProjMat;
		float m_playerPosZ;
	};
	struct OutputData
	{
		DirectX::XMMATRIX m_mat;
		DirectX::XMFLOAT4 m_color;
	};

	std::vector<KazBufferHelper::BufferData> m_computeInitBuffer;
	std::vector<KazBufferHelper::BufferData> m_computeUpdateBuffer;

	ComputeShader m_computeInit,m_computeUpdate;

	std::shared_ptr<KazBufferHelper::BufferData>m_particleVertexBuffer;
	std::shared_ptr<KazBufferHelper::BufferData>m_particleIndexBuffer;

	DrawFuncData::DrawCallData m_drawCall;


	KazBufferHelper::BufferData matrixBuffer;
	std::vector<DirectX::XMMATRIX> m_particleMatrix;


	std::array<DrawFuncData::DrawCallData, PARTICLE_MAX_NUM> m_playerModel;
};

