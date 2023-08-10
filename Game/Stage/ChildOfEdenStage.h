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
	void Draw(DrawingByRasterize& arg_rasterize)override;

public:
	KazMath::Transform3D m_skydormTransform;
	float m_skydormScale;
	DrawFuncData::DrawCallData m_drawSkydorm;

	//ステージ内に漂う三角パーティクル
	DrawFuncData::DrawCallData m_drawTriangleParticle;

	static const int PARTICLE_MAX_NUM = 1024 * 5;
	struct ParticeArgumentData
	{
		DirectX::XMFLOAT3 m_pos;
		DirectX::XMFLOAT3 m_rotation;
	};
	struct CameraBufferData
	{
		DirectX::XMMATRIX m_billboardMat;
		DirectX::XMMATRIX m_viewProjMat;
	};
	struct OutputData
	{
		DirectX::XMMATRIX mat;
		DirectX::XMFLOAT4 m_color;
	};

	std::vector<KazBufferHelper::BufferData> m_computeInitBuffer;
	std::vector<KazBufferHelper::BufferData> m_computeUpdateBuffer;

	ComputeShader m_computeInit,m_computeUpdate;
};

