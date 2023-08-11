#include "ChildOfEdenStage.h"

ChildOfEdenStage::ChildOfEdenStage() :m_skydormScale(100.0f)
{
	m_drawSkydorm = DrawFuncData::SetDrawGLTFIndexMaterialData(
		*ModelLoader::Instance()->Load(KazFilePathName::StagePath + "Skydorm/", "skydome.gltf"),
		DrawFuncData::GetModelShader()
	);
	m_skydormTransform.scale = { m_skydormScale,m_skydormScale,m_skydormScale };

	//パーティクル情報
	m_drawTriangleParticle.extraBufferArray.emplace_back(KazBufferHelper::SetGPUBufferData(sizeof(ParticeArgumentData) * PARTICLE_MAX_NUM, "Particle"));
	m_drawTriangleParticle.extraBufferArray[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_drawTriangleParticle.extraBufferArray[0].rootParamType = GRAPHICS_PRAMTYPE_DATA;

	//乱数テーブル生成
	m_randomTable = KazBufferHelper::SetUploadBufferData(sizeof(UINT) * PARTICLE_MAX_NUM,"RandomTable-UAV-UploadBuffer");
	std::array<UINT, PARTICLE_MAX_NUM>table;
	for (int i = 0; i < PARTICLE_MAX_NUM; ++i)
	{
		table[i] = KazMath::Rand<UINT>(1000000, 0);
	}
	m_randomTable.bufferWrapper->TransData(table.data(), sizeof(UINT) * PARTICLE_MAX_NUM);

	m_computeInitBuffer.emplace_back(m_drawTriangleParticle.extraBufferArray[0]);
	m_computeInitBuffer.emplace_back(m_randomTable);
	m_computeInitBuffer.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_computeInitBuffer.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
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
	m_computeUpdate.Generate(
		ShaderOptionData("Resource/ShaderFiles/ShaderFile/TriangleParticle.hlsl", "UpdateCSmain", "cs_6_4", SHADER_TYPE_COMPUTE),
		m_computeUpdateBuffer
	);

	m_computeUpdateBuffer[1].bufferWrapper->ChangeBarrier(
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	m_drawTriangleParticle =
		DrawFuncData::SetExecuteIndirect(
			DrawFuncData::GetBasicShader(),
			m_computeUpdateBuffer[1].bufferWrapper->GetBuffer()->GetGPUVirtualAddress(),
			PARTICLE_MAX_NUM
		);

	m_drawTriangleParticle.extraBufferArray.emplace_back(KazBufferHelper::SetGPUBufferData(sizeof(OutputData) * PARTICLE_MAX_NUM));
	m_drawTriangleParticle.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_drawTriangleParticle.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

	m_computeInit.Compute({ DISPATCH_MAX_NUM,1,1 });

}

void ChildOfEdenStage::Update()
{
}

void ChildOfEdenStage::Draw(DrawingByRasterize& arg_rasterize)
{
	////DrawFunc::DrawModel(m_drawSkydorm, m_skydormTransform);
	////arg_rasterize.ObjectRender(m_drawSkydorm);

	//arg_rasterize.ObjectRender(m_drawTriangleParticle);

	//CameraBufferData cameraMat;
	//cameraMat.m_billboardMat = CameraMgr::Instance()->GetMatBillBoard();
	//cameraMat.m_viewProjMat = CameraMgr::Instance()->GetViewMatrix() * CameraMgr::Instance()->GetPerspectiveMatProjection();
	//cameraMat.m_playerPosZ = playerPosZ;
	//m_computeUpdateBuffer[2].bufferWrapper->TransData(&cameraMat, sizeof(CameraBufferData));

	//m_computeUpdate.Compute({ DISPATCH_MAX_NUM,1,1 });
}