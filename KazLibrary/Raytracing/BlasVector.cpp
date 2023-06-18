#include "BlasVector.h"
#include "Blas.h"

namespace Raytracing
{

	BlasVector::BlasVector()
	{
	}

	void BlasVector::Update()
	{
	}

	void BlasVector::Add(std::weak_ptr<Blas> arg_refBlas, const DirectX::XMMATRIX& arg_worldMat)
	{

		/*===== Tlas�ɓo�^���邽�߂ɔz��ɒǉ����� =====*/

		//�Q�Ƃ�ǉ����āB
		m_refBlas.emplace_back(arg_refBlas);

		D3D12_RAYTRACING_INSTANCE_DESC instanceDesc;

		//�s���ݒ�B
		XMStoreFloat3x4(
			reinterpret_cast<DirectX::XMFLOAT3X4*>(&instanceDesc.Transform),
			arg_worldMat);

		//�C���X�^���X�̏ڍׂ�ݒ�B
		instanceDesc.InstanceID = 0;			//���C�g���ōs�������̃t���O�������Őݒ肷��B�}�e���A�����Őݒ肵�Ă��悢�B
		instanceDesc.InstanceMask = 0xFF;
		instanceDesc.InstanceContributionToHitGroupIndex = static_cast<int>(m_instanceDesc.size());
		instanceDesc.Flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		instanceDesc.AccelerationStructure = arg_refBlas.lock()->GetBlasBuffer()->GetGPUVirtualAddress();

		//�C���X�^���X��ǉ��B
		m_instanceDesc.emplace_back(instanceDesc);

	}

	int BlasVector::GetBlasRefCount()
	{
		return static_cast<int>(m_refBlas.size());
	}

	uint8_t* BlasVector::WriteShaderRecord(uint8_t* arg_dest, UINT arg_recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& arg_stateObject, LPCWSTR arg_hitGroup)
	{

		/*===== �V�F�[�_�[���R�[�h���������� =====*/

		for (auto& index : m_refBlas) {
			arg_dest = index.lock()->WriteShaderRecord(arg_dest, arg_recordSize, arg_stateObject, arg_hitGroup);
		}

		return arg_dest;

	}

}