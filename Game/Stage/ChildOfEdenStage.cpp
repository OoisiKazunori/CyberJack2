#include "ChildOfEdenStage.h"
#include "../KazLibrary/Raytracing/Blas.h"
#include"../KazLibrary/Buffer/ShaderRandomTable.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../KazLibrary/Easing/easing.h"

ChildOfEdenStage::ChildOfEdenStage() :m_skydormScale(100.0f)
{
	m_skydormTransform.scale = { m_skydormScale,m_skydormScale,m_skydormScale };

	m_drawTriangleParticle.extraBufferArray.emplace_back(KazBufferHelper::SetGPUBufferData(sizeof(ParticeArgumentData) * PARTICLE_MAX_NUM, "Particle"));
	m_drawTriangleParticle.extraBufferArray[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_drawTriangleParticle.extraBufferArray[0].rootParamType = GRAPHICS_PRAMTYPE_DATA;

	m_computeInitBuffer.emplace_back(m_drawTriangleParticle.extraBufferArray[0]);
	m_computeInitBuffer.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_computeInitBuffer.emplace_back(ShaderRandomTable::Instance()->GetBuffer(GRAPHICS_PRAMTYPE_DATA2));
	m_computeInit.Generate(
		ShaderOptionData("Resource/ShaderFiles/ShaderFile/TriangleParticle.hlsl", "InitCSmain", "cs_6_4", SHADER_TYPE_COMPUTE),
		m_computeInitBuffer
	);

	m_computeUpdateBuffer.emplace_back(m_drawTriangleParticle.extraBufferArray[0]);
	m_computeUpdateBuffer.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_computeUpdateBuffer.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

	m_computeUpdateBuffer.emplace_back(KazBufferHelper::SetGPUBufferData(sizeof(OutputData) * PARTICLE_MAX_NUM));
	m_computeUpdateBuffer.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_computeUpdateBuffer.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;

	m_computeUpdateBuffer.emplace_back(KazBufferHelper::SetConstBufferData(sizeof(CameraBufferData)));
	m_computeUpdateBuffer.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
	m_computeUpdateBuffer.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

	matrixVRAMBuffer = KazBufferHelper::BufferResourceData
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		CD3DX12_RESOURCE_DESC::Buffer(sizeof(DirectX::XMMATRIX) * PARTICLE_MAX_NUM),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		"Particle-Matrix-VRAM"
	);

	D3D12_HEAP_PROPERTIES prop = {};
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	prop.CreationNodeMask = 1;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	prop.Type = D3D12_HEAP_TYPE_CUSTOM;
	prop.VisibleNodeMask = 1;
	matrixBuffer = KazBufferHelper::BufferResourceData
	(
		prop,
		D3D12_HEAP_FLAG_NONE,
		CD3DX12_RESOURCE_DESC::Buffer(sizeof(DirectX::XMMATRIX) * PARTICLE_MAX_NUM, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		"Particle-Matrix"
	);
	matrixBuffer.bufferWrapper->ChangeBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	m_particleMatrix.resize(PARTICLE_MAX_NUM);
	m_computeUpdateBuffer.emplace_back(matrixBuffer);
	m_computeUpdateBuffer.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_computeUpdateBuffer.back().rootParamType = GRAPHICS_PRAMTYPE_DATA3;

	m_computeUpdate.Generate(
		ShaderOptionData("Resource/ShaderFiles/ShaderFile/TriangleParticle.hlsl", "UpdateCSmain", "cs_6_4", SHADER_TYPE_COMPUTE),
		m_computeUpdateBuffer
	);

	m_computeUpdateBuffer[1].bufferWrapper->ChangeBarrier(
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	m_drawTriangleParticle =
		DrawFuncData::SetExecuteIndirect(
			DrawFuncData::GetBasicInstanceShader(),
			m_computeUpdateBuffer[1].bufferWrapper->GetBuffer()->GetGPUVirtualAddress(),
			PARTICLE_MAX_NUM
		);

	m_drawTriangleParticle.extraBufferArray.emplace_back(KazBufferHelper::SetGPUBufferData(sizeof(OutputData) * PARTICLE_MAX_NUM));
	m_drawTriangleParticle.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_drawTriangleParticle.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

	m_computeInit.Compute({ DISPATCH_MAX_NUM,1,1 });

	auto playerModel = *ModelLoader::Instance()->Load("Resource/Player/", "Player.gltf");
	auto pipeline = DrawFuncData::GetModelBloomShader();
	//m_playerModel.resize(PARTICLE_MAX_NUM);
	for (auto &index : m_playerModel)
	{
		index = DrawFuncData::SetRaytracingData(playerModel, pipeline);
	}

	for (int i = 0; i < m_radius.size(); ++i)
	{
		m_radius[i] = 0.0f;
	}
}

void ChildOfEdenStage::Update()
{
}

void ChildOfEdenStage::Draw(DrawingByRasterize &arg_rasterize, Raytracing::BlasVector &arg_blasVec)
{
	//GPUパーティクルをCPUに書き込む処理を追加
	memcpy(m_particleMatrix.data(), matrixBuffer.bufferWrapper->GetMapAddres(), sizeof(DirectX::XMMATRIX) * PARTICLE_MAX_NUM);

	for (auto &blas : m_playerModel.back().m_raytracingData.m_blas)
	{
		arg_blasVec.AddVector(blas, m_particleMatrix, 1, 0xFE);
	}

	arg_rasterize.ObjectRender(m_drawTriangleParticle);


	CameraBufferData cameraMat;
	cameraMat.m_billboardMat = CameraMgr::Instance()->GetMatBillBoard();
	cameraMat.m_viewProjMat = CameraMgr::Instance()->GetViewMatrix() * CameraMgr::Instance()->GetPerspectiveMatProjection();
	for (int i = 0; i < hitFlag.size(); ++i)
	{
		Rate(&m_rate[i], 0.01f, 1.0f);
		if (hitFlag[i])
		{
			m_radius[i] += 60.0f;
		}
		else
		{
			m_radius[i] = 0.0f;
			m_rate[i] = 0.0f;
		}

		if (4000.0f <= m_radius[i])
		{
			hitFlag[i] = false;
		}
	}
	cameraMat.posZ1 = m_radius[0];
	cameraMat.posZ2 = m_radius[1];
	cameraMat.posZ3 = m_radius[2];
	cameraMat.posZ4 = m_radius[3];
	cameraMat.posZ5 = m_radius[4];
	cameraMat.posZ6 = m_radius[5];
	cameraMat.posZ7 = m_radius[6];
	cameraMat.posZ8 = m_radius[7];
	cameraMat.posZ9 = m_radius[8];
	cameraMat.posZ10 = m_radius[9];
	cameraMat.posZ11 = m_radius[10];
	cameraMat.posZ12 = m_radius[11];
	cameraMat.posZ13 = m_radius[12];
	cameraMat.posZ14 = m_radius[13];
	cameraMat.posZ15 = m_radius[14];
	cameraMat.posZ16 = m_radius[15];

	m_computeUpdateBuffer[2].bufferWrapper->TransData(&cameraMat, sizeof(CameraBufferData));

	m_computeUpdate.Compute({ DISPATCH_MAX_NUM,1,1 });
}