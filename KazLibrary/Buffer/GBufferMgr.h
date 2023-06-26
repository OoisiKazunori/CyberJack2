#pragma once
#include"../KazLibrary/Helper/ISinglton.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Math/KazMath.h"

/// <summary>
/// G-Buffer�̊Ǘ��N���X
/// </summary>
class GBufferMgr :public ISingleton<GBufferMgr>
{
public:
	enum BufferType
	{
		NONE = -1,
		ALBEDO,
		NORMAL,
		R_M_S_ID,
		WORLD,
		MAX
	};
	GBufferMgr();

	std::vector<RESOURCE_HANDLE> GetRenderTarget();
	std::vector<DXGI_FORMAT> GetRenderTargetFormat();
	void SetCameraPos(DirectX::XMFLOAT3 arg_pos)
	{
		m_cameraPosBuffer.bufferWrapper->TransData(&arg_pos,sizeof(DirectX::XMFLOAT3));
	};
	const KazBufferHelper::BufferData &GetEyePosBuffer()
	{
		return m_cameraPosBuffer;
	}


	/// <summary>
	/// �f�o�b�N�p�̕`��m�F
	/// </summary>
	void DebugDraw();

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(BufferType arg_type);

	const KazBufferHelper::BufferData &GetFinalBuffer()
	{
		return m_finalGBuffer;
	};

	const KazBufferHelper::BufferData &GetRayTracingBuffer()
	{
		return m_raytracingGBuffer;
	};

	//���C�g�p�\����
	struct DirLight {
		KazMath::Vec3<float> m_dir;
		int m_isActive;
	};
	struct PointLight {
		KazMath::Vec3<float> m_pos;
		float m_power;		//���C�g���e����^����ő勗��
		KazMath::Vec3<float> m_pad;	//�p�����[�^�[���K�v�ɂȂ�����K���ɖ��O���ĕϐ���ǉ����Ă��������I
		int m_isActive;
	};
	struct LightConstData {
		DirLight m_dirLight;
		PointLight m_pointLight;
	}m_lightConstData;

	KazBufferHelper::BufferData m_lightBuffer;

private:
	//G-Buffer�p�̃����_�[�^�[�Q�b�g
	std::vector<RESOURCE_HANDLE>m_gBufferRenderTargetHandleArray;
	std::vector<DXGI_FORMAT>m_gBufferFormatArray;

	KazBufferHelper::BufferData m_cameraPosBuffer;

	//�ŏI��������
	KazBufferHelper::BufferData m_finalGBuffer;
	KazBufferHelper::BufferData m_raytracingGBuffer;
};

