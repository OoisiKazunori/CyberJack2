#include "AnimationInRaytracing.h"

AnimationInRaytracing::AnimationInRaytracing()
{
	RootSignatureDataTest test;
	test.rangeArray.emplace_back(GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA);
	test.rangeArray.emplace_back(GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA2);
	test.rangeArray.emplace_back(GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA);
	test.rangeArray.emplace_back(GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA2);
	m_compute.Generate(ShaderOptionData("Resource/ShaderFiles/ShaderFile/Animation.hlsl", "CSmain", "cs_6_5", SHADER_TYPE_COMPUTE), test);
	m_compute.m_extraBufferArray.emplace_back();
	m_compute.m_extraBufferArray.emplace_back();
	m_compute.m_extraBufferArray.emplace_back();
	m_compute.m_extraBufferArray.emplace_back(KazBufferHelper::SetConstBufferData(sizeof(CommonData)));
}

void AnimationInRaytracing::Compute(const KazBufferHelper::BufferData& arg_vertexBuffer, const KazBufferHelper::BufferData& arg_boneBuffer, const DirectX::XMMATRIX& arg_worldMat)
{
	m_compute.m_extraBufferArray[0] = m_vertexBuffer;
	m_compute.m_extraBufferArray[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_compute.m_extraBufferArray[0].rootParamType = GRAPHICS_PRAMTYPE_DATA;

	m_compute.m_extraBufferArray[1] = arg_vertexBuffer;
	m_compute.m_extraBufferArray[1].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_compute.m_extraBufferArray[1].rootParamType = GRAPHICS_PRAMTYPE_DATA2;

	m_compute.m_extraBufferArray[2] = arg_boneBuffer;
	m_compute.m_extraBufferArray[2].rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
	m_compute.m_extraBufferArray[2].rootParamType = GRAPHICS_PRAMTYPE_DATA;

	CommonData data;
	data.vertNum = m_vertexBuffer.elementNum;
	data.m_worldMat = arg_worldMat;
	data.m_viewProj = CameraMgr::Instance()->GetViewMatrix() * CameraMgr::Instance()->GetPerspectiveMatProjection();

	m_compute.m_extraBufferArray[3].bufferWrapper->TransData(&data, sizeof(CommonData));
	m_compute.m_extraBufferArray[3].rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
	m_compute.m_extraBufferArray[3].rootParamType = GRAPHICS_PRAMTYPE_DATA2;
	UINT num = data.vertNum / 1024;



	arg_vertexBuffer.bufferWrapper->ChangeBarrier(
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);
	/*arg_indexBuffer.bufferWrapper->ChangeBarrier(
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);*/

	arg_vertexBuffer.bufferWrapper->ChangeBarrierUAV();
	//arg_indexBuffer.bufferWrapper->ChangeBarrierUAV();
	m_compute.Compute({ num + 1,1,1 });
	arg_vertexBuffer.bufferWrapper->ChangeBarrierUAV();
	//arg_indexBuffer.bufferWrapper->ChangeBarrierUAV();

	arg_vertexBuffer.bufferWrapper->ChangeBarrier(
		D3D12_RESOURCE_STATE_COMMON
	);
	/*arg_indexBuffer.bufferWrapper->ChangeBarrier(
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COMMON
	);*/
}

void AnimationInRaytracing::GenerateBuffer(const KazBufferHelper::BufferData& arg_vertexBuffer)
{
	m_vertexBuffer = KazBufferHelper::SetGPUBufferData(arg_vertexBuffer.structureSize * arg_vertexBuffer.elementNum);
	m_vertexBuffer.bufferWrapper->CopyBuffer(arg_vertexBuffer.bufferWrapper->GetBuffer());
	m_vertexBuffer.elementNum = arg_vertexBuffer.elementNum;
	m_vertexBuffer.structureSize = arg_vertexBuffer.structureSize;
}
