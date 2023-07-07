#include "GBufferMgr.h"
#include"../KazLibrary/Buffer/UavViewHandleMgr.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"RenderTarget/RenderTargetStatus.h"

//���[���h���W�A���t�l�X�A���^���l�X�A�X�؃L�����A�I�u�W�F�N�g�����˂��邩���܂��邩(�C���f�b�N�X)�AAlbedo�A�@���A�J�������W(�萔�o�b�t�@�ł��\)
GBufferMgr::GBufferMgr()
{
	KazMath::Vec2<UINT>winSize(1280, 720);

	//G-Buffer�p�̃����_�[�^�[�Q�b�g����
	{
		std::vector<MultiRenderTargetData> multiRenderTargetArray(MAX);
		m_gBufferFormatArray.resize(MAX);
		m_gBufferFormatArray[ALBEDO] = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_gBufferFormatArray[NORMAL] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		m_gBufferFormatArray[R_M_S_ID] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		m_gBufferFormatArray[WORLD] = DXGI_FORMAT_R32G32B32A32_FLOAT;

		//�A���x�h
		multiRenderTargetArray[ALBEDO].backGroundColor = { 0.0f,0.0f,0.0f };
		multiRenderTargetArray[ALBEDO].graphSize = winSize;
		multiRenderTargetArray[ALBEDO].format = m_gBufferFormatArray[ALBEDO];
		//�m�[�}��
		multiRenderTargetArray[NORMAL].backGroundColor = { 0.0f,0.0f,0.0f };
		multiRenderTargetArray[NORMAL].graphSize = winSize;
		multiRenderTargetArray[NORMAL].format = m_gBufferFormatArray[NORMAL];
		//���t�l�X�A���^���l�X�A�X�؃L�����A���ܔ���(0...���˂��Ȃ��A1...���˂���A2...���܂���)
		multiRenderTargetArray[R_M_S_ID].backGroundColor = { 0.0f,0.0f,0.0f };
		multiRenderTargetArray[R_M_S_ID].graphSize = winSize;
		multiRenderTargetArray[R_M_S_ID].format = m_gBufferFormatArray[R_M_S_ID];
		//���[���h���W
		multiRenderTargetArray[WORLD].backGroundColor = { 0.0f,0.0f,0.0f };
		multiRenderTargetArray[WORLD].graphSize = winSize;
		multiRenderTargetArray[WORLD].format = m_gBufferFormatArray[WORLD];

		m_gBufferRenderTargetHandleArray = RenderTargetStatus::Instance()->CreateMultiRenderTarget(multiRenderTargetArray);

		m_finalGBuffer = KazBufferHelper::SetUAVTexBuffer(winSize.x, winSize.y);
		m_finalGBuffer.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
		DescriptorHeapMgr::Instance()->CreateBufferView(
			m_finalGBuffer.bufferWrapper->GetViewHandle(),
			KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), winSize.x * winSize.y),
			m_finalGBuffer.bufferWrapper->GetBuffer().Get()
		);

		m_raytracingGBuffer = KazBufferHelper::SetUAVTexBuffer(winSize.x, winSize.y, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_raytracingGBuffer.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
		DescriptorHeapMgr::Instance()->CreateBufferView(
			m_raytracingGBuffer.bufferWrapper->GetViewHandle(),
			KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), winSize.x * winSize.y),
			m_raytracingGBuffer.bufferWrapper->GetBuffer().Get()
		);
	}

	m_cameraPosBuffer = KazBufferHelper::SetConstBufferData(sizeof(CameraEyePosBufferData));
	m_lightBuffer = KazBufferHelper::SetConstBufferData(sizeof(LightConstData));
	m_lightConstData.m_dirLight.m_dir = KazMath::Vec3<float>(0.0f, -1.0f, 0.0f);
	m_lightConstData.m_pointLight.m_power = 100.0f;

	//�{�����[���e�N�X�`���𐶐��B
	m_volumeFogTextureBuffer = KazBufferHelper::SetUAV3DTexBuffer(256, 256, 256, DXGI_FORMAT_R8G8B8A8_UNORM);
	m_volumeFogTextureBuffer.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
	DescriptorHeapMgr::Instance()->CreateBufferView(
		m_volumeFogTextureBuffer.bufferWrapper->GetViewHandle(),
		KazBufferHelper::SetUnorderedAccess3DTextureView(sizeof(DirectX::XMFLOAT4), 256 * 256 * 256),
		m_volumeFogTextureBuffer.bufferWrapper->GetBuffer().Get()
	);
}

std::vector<RESOURCE_HANDLE> GBufferMgr::GetRenderTarget()
{
	return m_gBufferRenderTargetHandleArray;
}

std::vector<DXGI_FORMAT> GBufferMgr::GetRenderTargetFormat()
{
	return m_gBufferFormatArray;
}

D3D12_GPU_DESCRIPTOR_HANDLE GBufferMgr::GetGPUHandle(BufferType arg_type)
{
	RESOURCE_HANDLE handle = RenderTargetStatus::Instance()->GetBuffer(m_gBufferRenderTargetHandleArray[arg_type]).bufferWrapper->GetViewHandle();
	return DescriptorHeapMgr::Instance()->GetGpuDescriptorView(handle);
}
