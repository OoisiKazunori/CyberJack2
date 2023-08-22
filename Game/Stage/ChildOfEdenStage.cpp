#include "ChildOfEdenStage.h"
#include "../KazLibrary/Raytracing/Blas.h"
#include"../KazLibrary/Buffer/ShaderRandomTable.h"


ChildOfEdenStage::ChildOfEdenStage() :m_skydormScale(100.0f)
{
	//m_drawSkydorm = DrawFuncData::SetDrawGLTFIndexMaterialData(
	//	*ModelLoader::Instance()->Load(KazFilePathName::StagePath + "Skydorm/", "skydome.gltf"),
	//	DrawFuncData::GetModelShader()
	//);
	m_skydormTransform.scale = { m_skydormScale,m_skydormScale,m_skydormScale };

	m_drawTriangleParticle.extraBufferArray.emplace_back(KazBufferHelper::SetGPUBufferData(sizeof(ParticeArgumentData) * PARTICLE_MAX_NUM, "Particle"));
	m_drawTriangleParticle.extraBufferArray[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_drawTriangleParticle.extraBufferArray[0].rootParamType = GRAPHICS_PRAMTYPE_DATA;

	m_computeInitBuffer.emplace_back(m_drawTriangleParticle.extraBufferArray[0]);
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


	m_particleVertexBuffer = std::make_shared<KazBufferHelper::BufferData>(KazBufferHelper::SetGPUBufferData((sizeof(VertexBufferData) * PARTICLE_MAX_NUM) * 4, "GPUParticle-VertexBuffer"));
	m_particleVertexBuffer->structureSize = sizeof(VertexBufferData);
	m_particleVertexBuffer->elementNum = PARTICLE_MAX_NUM * 4;

	m_particleIndexBuffer = std::make_shared<KazBufferHelper::BufferData>(KazBufferHelper::SetGPUBufferData((sizeof(UINT) * PARTICLE_MAX_NUM) * 6, "GPUParticle-IndexBuffer"));
	m_particleIndexBuffer->structureSize = sizeof(UINT);
	m_particleIndexBuffer->elementNum = PARTICLE_MAX_NUM * 6;

	m_drawTriangleParticleInRaytracing = DrawFuncData::SetParticleInRaytracing(
		m_particleVertexBuffer,
		m_particleIndexBuffer
	);
	m_particleVertexBuffer->bufferWrapper->ChangeBarrier(
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);
	m_particleIndexBuffer->bufferWrapper->ChangeBarrier(
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	m_computeUpdateBuffer.emplace_back(*m_particleVertexBuffer);
	m_computeUpdateBuffer.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_computeUpdateBuffer.back().rootParamType = GRAPHICS_PRAMTYPE_DATA3;

	m_computeUpdateBuffer.emplace_back(*m_particleIndexBuffer);
	m_computeUpdateBuffer.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_computeUpdateBuffer.back().rootParamType = GRAPHICS_PRAMTYPE_DATA4;

	matrixBuffer = KazBufferHelper::SetUploadBufferData(sizeof(DirectX::XMMATRIX) * PARTICLE_MAX_NUM, "Particle-Matrix");
	m_computeUpdateBuffer.emplace_back(matrixBuffer);
	m_computeUpdateBuffer.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_computeUpdateBuffer.back().rootParamType = GRAPHICS_PRAMTYPE_DATA5;
	
	m_computeUpdate.Generate(
		ShaderOptionData("Resource/ShaderFiles/ShaderFile/TriangleParticle.hlsl", "UpdateCSmain", "cs_6_4", SHADER_TYPE_COMPUTE),
		m_computeUpdateBuffer
	);

	m_computeUpdateBuffer[1].bufferWrapper->ChangeBarrier(
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON
	);

	m_drawTriangleParticle =
		DrawFuncData::SetExecuteIndirect(
			DrawFuncData::GetBasicInstanceShader(),
			m_computeUpdateBuffer[1].bufferWrapper->GetBuffer()->GetGPUVirtualAddress(),
			PARTICLE_MAX_NUM,
			PARTICLE_MAX_NUM * 6
		);

	m_drawTriangleParticle.extraBufferArray.emplace_back(KazBufferHelper::SetGPUBufferData(sizeof(OutputData) * PARTICLE_MAX_NUM));
	m_drawTriangleParticle.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_drawTriangleParticle.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

	m_computeInit.Compute({ DISPATCH_MAX_NUM,1,1 });

	KazRenderHelper::DrawIndexInstanceCommandData command;
	//topology
	command.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//indexinstance
	command.drawIndexInstancedData = { PARTICLE_MAX_NUM * 6,1,0,0,0 };
	//view
	command.vertexBufferDrawData.slot = 0;
	command.vertexBufferDrawData.numViews = 1;
	command.vertexBufferDrawData.vertexBufferView =
		KazBufferHelper::SetVertexBufferView(
			m_particleVertexBuffer->bufferWrapper->GetGpuAddress(),
			sizeof(VertexBufferData) * (PARTICLE_MAX_NUM * 4),
			sizeof(VertexBufferData)
		);
	command.indexBufferView = KazBufferHelper::SetIndexBufferView(m_particleIndexBuffer->bufferWrapper->GetGpuAddress(), sizeof(UINT) * (PARTICLE_MAX_NUM * 6));

	
	command.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_drawTriangleParticle.drawMultiMeshesIndexInstanceCommandData.drawIndexInstancedData[0] = command.drawIndexInstancedData;
	m_drawTriangleParticle.drawMultiMeshesIndexInstanceCommandData.vertexBufferDrawData[0] = command.vertexBufferDrawData;
	m_drawTriangleParticle.drawMultiMeshesIndexInstanceCommandData.indexBufferView[0] = command.indexBufferView;
	m_drawCall = DrawFuncData::SetDrawPolygonIndexData(command, DrawFuncData::GetBasicShader());
}

void ChildOfEdenStage::Update()
{
	m_drawTriangleParticleInRaytracing.m_raytracingData.m_blas[0]->Update();
}

void ChildOfEdenStage::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{
	//DrawFunc::DrawModel(m_drawSkydorm, m_skydormTransform);
	//arg_rasterize.ObjectRender(m_drawSkydorm);

	arg_rasterize.ObjectRender(m_drawTriangleParticle);

	for (auto& index : m_drawTriangleParticleInRaytracing.m_raytracingData.m_blas)
	{
		arg_blasVec.Add(index, DirectX::XMMatrixIdentity(), 1);
	}

	CameraBufferData cameraMat;
	cameraMat.m_billboardMat = CameraMgr::Instance()->GetMatBillBoard();
	cameraMat.m_viewProjMat = CameraMgr::Instance()->GetViewMatrix() * CameraMgr::Instance()->GetPerspectiveMatProjection();
	cameraMat.m_playerPosZ = playerPosZ;
	m_computeUpdateBuffer[2].bufferWrapper->TransData(&cameraMat, sizeof(CameraBufferData));

	m_particleVertexBuffer->bufferWrapper->ChangeBarrier(
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);
	m_particleIndexBuffer->bufferWrapper->ChangeBarrier(
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	m_particleVertexBuffer->bufferWrapper->ChangeBarrierUAV();
	m_particleIndexBuffer->bufferWrapper->ChangeBarrierUAV();
	m_computeUpdate.Compute({ DISPATCH_MAX_NUM,1,1 });
	m_particleVertexBuffer->bufferWrapper->ChangeBarrierUAV();
	m_particleIndexBuffer->bufferWrapper->ChangeBarrierUAV();

	m_particleVertexBuffer->bufferWrapper->ChangeBarrier(
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
	);
	m_particleIndexBuffer->bufferWrapper->ChangeBarrier(
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
	);
	//arg_rasterize.ObjectRender(m_drawCall);
}