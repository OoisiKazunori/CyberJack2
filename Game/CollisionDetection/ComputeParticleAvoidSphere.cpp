#include "ComputeParticleAvoidSphere.h"
#include"../KazLibrary/Render/GPUParticleRender.h"
#include"../Game/Effect/InstanceMeshParticle.h"

ComputeParticleAvoidSphere::ComputeParticleAvoidSphere()
{
	//�p�[�e�B�N�����̔���
	meshCircleArrayBufferHandle = m_computeHelper.CreateBuffer(
		KazBufferHelper::SetOnlyReadStructuredBuffer(sizeof(MeshHitBoxData) * PARTICLE_MAX_NUM),
		GRAPHICS_RANGE_TYPE_UAV_DESC,
		GRAPHICS_PRAMTYPE_DATA,
		sizeof(MeshHitBoxData),
		PARTICLE_MAX_NUM,
		true);

	UINT lNum = 0;
	KazBufferHelper::BufferResourceData lBufferData
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		"CopyCounterBuffer"
	);
	copyBuffer.CreateBuffer(lBufferData);
	copyBuffer.TransData(&lNum, sizeof(UINT));

	m_computeHelper.InitCounterBuffer(copyBuffer.GetBuffer());

	//���W�̃��[�v����
	RESOURCE_HANDLE lHandle = m_computeHelper.CreateBuffer(
		KazBufferHelper::SetOnlyReadStructuredBuffer(sizeof(DirectX::XMFLOAT3) * PARTICLE_MAX_NUM),
		GRAPHICS_RANGE_TYPE_UAV_DESC,
		GRAPHICS_PRAMTYPE_DATA4,
		sizeof(DirectX::XMFLOAT3),
		PARTICLE_MAX_NUM);

}

void ComputeParticleAvoidSphere::SetHitIDBuffer(const ResouceBufferHelper::BufferData &HIT_ID_BUFFER)
{
	//�����̃��b�V�����Ɣ�����������
	m_computeHelper.SetBuffer(HIT_ID_BUFFER, GRAPHICS_PRAMTYPE_DATA2);

	//���W�ƃ��[�v����g�ݍ��킹�����[���h�s��
	outputParticleBufferHandle = m_computeHelper.CreateBuffer(
		KazBufferHelper::SetOnlyReadStructuredBuffer(sizeof(InitOutputData) * PARTICLE_MAX_NUM),
		GRAPHICS_RANGE_TYPE_UAV_DESC,
		GRAPHICS_PRAMTYPE_DATA3,
		sizeof(InitOutputData),
		PARTICLE_MAX_NUM,
		false);

}

void ComputeParticleAvoidSphere::GenerateHitNum(UINT NUM)
{
	RESOURCE_HANDLE lHandle = m_computeHelper.CreateBuffer(
		sizeof(UINT),
		GRAPHICS_RANGE_TYPE_CBV_VIEW,
		GRAPHICS_PRAMTYPE_DATA5,
		1,
		false);

	UINT lNum = static_cast<UINT>(NUM);
	m_computeHelper.TransData(lHandle, &lNum, sizeof(UINT));
}

void ComputeParticleAvoidSphere::Compute()
{
	m_computeHelper.InitCounterBuffer(copyBuffer.GetBuffer());
	m_computeHelper.StackToCommandListAndCallDispatch(PIPELINE_COMPUTE_NAME_HITBOX_AVOID_PARTICLE, { 1000,1,1 });
}

const ResouceBufferHelper::BufferData &ComputeParticleAvoidSphere::GetStackParticleHitBoxBuffer()
{
	return m_computeHelper.GetBufferData(meshCircleArrayBufferHandle);
}

const ResouceBufferHelper::BufferData &ComputeParticleAvoidSphere::GetOutputParticleData()
{
	return m_computeHelper.GetBufferData(outputParticleBufferHandle);
}
