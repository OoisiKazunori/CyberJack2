#include "EnemyDeadParticle.h"

EnemyDeadParticle::EnemyDeadParticle(const KazBufferHelper::BufferData& arg_meshEmitterBuffer, const KazMath::Vec3<float>& arg_pos)
{
	//コンピュートシェーダーの生成
	{
		RootSignatureDataTest rootSignature;
		rootSignature.rangeArray.emplace_back(GRAPHICS_RANGE_TYPE_UAV_DESC, GRAPHICS_PRAMTYPE_DATA);
		rootSignature.rangeArray.emplace_back(GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA2);
		rootSignature.rangeArray.emplace_back(GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA3);
		rootSignature.rangeArray.emplace_back(GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA);
		m_init.Generate(ShaderOptionData("Resource/ShaderFiles/ShaderFile/DeadParticle.hlsl", "InitCSmain", "cs_6_4"), rootSignature);
	}
	{
		RootSignatureDataTest rootSignature;
		rootSignature.rangeArray.emplace_back(GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA);
		rootSignature.rangeArray.emplace_back(GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA2);
		rootSignature.rangeArray.emplace_back(GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA3);
		rootSignature.rangeArray.emplace_back(GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA);
		m_update.Generate(ShaderOptionData("Resource/ShaderFiles/ShaderFile/DeadParticle.hlsl", "UpdateCSmain", "cs_6_4"), rootSignature);
	}

	//生成予定のバッファ
	m_emitterBuffer = KazBufferHelper::BufferData(KazBufferHelper::SetGPUBufferData(sizeof(Particle) * PARTICLE_MAX_NUM), GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA2);
	m_outputBuffer = KazBufferHelper::BufferData(KazBufferHelper::SetGPUBufferData(sizeof(OutputData) * PARTICLE_MAX_NUM), GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA);

	m_initCommonBuffer = KazBufferHelper::BufferData(KazBufferHelper::SetConstBufferData(sizeof(InitCommonData)), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA);
	m_updateCommonBuffer = KazBufferHelper::BufferData(KazBufferHelper::SetConstBufferData(sizeof(UpdateCommonData)), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA);

	//バッファ生成
	m_init.m_extraBufferArray.emplace_back(arg_meshEmitterBuffer);
	m_init.m_extraBufferArray.emplace_back(m_emitterBuffer);
	m_init.m_extraBufferArray.emplace_back(ShaderRandomTable::Instance()->GetBuffer(GRAPHICS_PRAMTYPE_DATA3));
	m_init.m_extraBufferArray.emplace_back(m_initCommonBuffer);

	m_update.m_extraBufferArray.emplace_back(m_outputBuffer);
	m_update.m_extraBufferArray.emplace_back(m_emitterBuffer);
	m_update.m_extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
	m_update.m_extraBufferArray.emplace_back(ShaderRandomTable::Instance()->GetCurlBuffer(GRAPHICS_PRAMTYPE_DATA3));
	m_update.m_extraBufferArray.emplace_back(m_updateCommonBuffer);

	m_executeIndirect = DrawFuncData::SetExecuteIndirect(
		DrawFuncData::GetBasicInstanceEmissiveShader(),
		m_outputBuffer.bufferWrapper->GetBuffer()->GetGPUVirtualAddress(),
		PARTICLE_MAX_NUM
	);
	m_executeIndirect.extraBufferArray.emplace_back(m_outputBuffer);
	m_executeIndirect.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_executeIndirect.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

}

void EnemyDeadParticle::InitCompute(const KazMath::Vec3<float>& arg_pos, int particleNum)
{
	m_timer = 0;

	InitCommonData num;
	num.m_particleNum = static_cast<float>(particleNum);
	num.pos = arg_pos.ConvertXMFLOAT3();
	m_pos = arg_pos;

	m_initCommonBuffer.bufferWrapper->TransData(&num, sizeof(InitCommonData));
	//初期化シェーダーを通す
	m_init.Compute({ 1,1,1 });
}

void EnemyDeadParticle::UpdateCompute(DrawingByRasterize& arg_rasterize)
{
	++m_timer;

	UpdateCommonData data;
	data.m_billboard = CameraMgr::Instance()->GetMatBillBoard();
	data.m_viewProjMat = CameraMgr::Instance()->GetViewMatrix() * CameraMgr::Instance()->GetPerspectiveMatProjection();
	float scale = 0.25f;
	data.m_scaleRotaMat = KazMath::CaluScaleMatrix(KazMath::Vec3<float>(scale, scale, scale));
	data.emittPosAndEmittTimer = m_pos.ConvertXMFLOAT4();
	data.emittPosAndEmittTimer.w = static_cast<float>(m_timer);
	data.color = m_color.ConvertColorRateToXMFLOAT4();
	data.m_vel = m_vel;
	m_updateCommonBuffer.bufferWrapper->TransData(&data, sizeof(UpdateCommonData));
	//更新シェーダーを通す
	m_update.Compute({ 1,1,1 });

	arg_rasterize.ObjectRender(m_executeIndirect);
}
