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
	//DrawFuncData::DrawCallData m_drawTriangleParticleInRaytracing;

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
		float m_posZ1;
		float m_posZ2;
		float m_posZ3;
		float m_posZ4;
		float m_posZ5;
		float m_posZ6;
		float m_posZ7;
		float m_posZ8;
		UINT num;
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
	std::array<float,8> m_radius;
	std::array<float,8> m_rate;
};

