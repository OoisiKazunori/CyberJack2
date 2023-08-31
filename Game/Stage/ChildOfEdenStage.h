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

	//ステージ内に漂う三角パーティクル
	DrawFuncData::DrawCallData m_drawTriangleParticle;

	static const int DISPATCH_MAX_NUM = 10;
	static const int PARTICLE_MAX_NUM = 1024 * DISPATCH_MAX_NUM;
	struct ParticeArgumentData
	{
		DirectX::XMFLOAT3 m_pos;
		DirectX::XMFLOAT3 m_basePos;
		DirectX::XMFLOAT3 m_scale;
		DirectX::XMFLOAT3 m_rotation;
		DirectX::XMFLOAT3 m_rotationVel;
		DirectX::XMFLOAT4 m_color;
		DirectX::XMFLOAT3 m_posLerp;
		DirectX::XMFLOAT3 m_rotationLerp;
		DirectX::XMFLOAT4 m_colorLerp;
		DirectX::XMFLOAT3 m_scaleLerp;
		int m_timer;
		int isHitFlag;
	};
	struct CameraBufferData
	{
		DirectX::XMMATRIX m_billboardMat;
		DirectX::XMMATRIX m_viewProjMat;
		float posZ1;
		float posZ2;
		float posZ3;
		float posZ4;
		float posZ5;
		float posZ6;
		float posZ7;
		float posZ8;
		float posZ9;
		float posZ10;
		float posZ11;
		float posZ12;
		float posZ13;
		float posZ14;
		float posZ15;
		float posZ16;
	};
	struct OutputData
	{
		DirectX::XMMATRIX m_mat;
		DirectX::XMFLOAT4 m_color;
	};

	std::vector<KazBufferHelper::BufferData> m_computeInitBuffer;
	std::vector<KazBufferHelper::BufferData> m_computeUpdateBuffer;

	ComputeShader m_computeInit,m_computeUpdate;

	KazBufferHelper::BufferData matrixVRAMBuffer;
	KazBufferHelper::BufferData matrixBuffer;
	std::vector<DirectX::XMMATRIX> m_particleMatrix;


	std::array<DrawFuncData::DrawCallData, PARTICLE_MAX_NUM> m_playerModel;
	std::array<float,16> m_radius;
	std::array<float,16> m_rate;
};

