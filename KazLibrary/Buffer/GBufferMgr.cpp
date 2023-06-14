#include "GBufferMgr.h"
#include"../KazLibrary/Buffer/UavViewHandleMgr.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

//���[���h���W�A���t�l�X�A���^���l�X�A�X�؃L�����A�I�u�W�F�N�g�����˂��邩���܂��邩(�C���f�b�N�X)�AAlbedo�A�@���A�J�������W(�萔�o�b�t�@�ł��\)
GBufferMgr::GBufferMgr()
{
	KazMath::Vec2<int>winSize(1280, 720);

	//Albedo
	{
		KazBufferHelper::BufferData buffer(KazBufferHelper::SetUAVTexBuffer(winSize.x, winSize.y, "G-Buffer_Albedo"));
		RESOURCE_HANDLE view = UavViewHandleMgr::Instance()->GetHandle();

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = winSize.x * winSize.y;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		DescriptorHeapMgr::Instance()->CreateBufferView(view, uavDesc, buffer.bufferWrapper->GetBuffer().Get());
		buffer.bufferWrapper->CreateViewHandle(view);
		buffer.elementNum = winSize.x * winSize.y;
		buffer.rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;

		m_gBufferArray.emplace_back(buffer);
	}

	//Normal
	{

		KazBufferHelper::BufferData buffer(KazBufferHelper::SetUAVTexBuffer(winSize.x, winSize.y, "G-Buffer_Normal"));
		RESOURCE_HANDLE view = UavViewHandleMgr::Instance()->GetHandle();

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = winSize.x * winSize.y;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		DescriptorHeapMgr::Instance()->CreateBufferView(view, uavDesc, buffer.bufferWrapper->GetBuffer().Get());
		buffer.bufferWrapper->CreateViewHandle(view);
		buffer.elementNum = winSize.x * winSize.y;
		buffer.rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		buffer.rootParamType = GRAPHICS_PRAMTYPE_DATA3;

		m_gBufferArray.emplace_back(buffer);
	}
	//final
	{
		KazBufferHelper::BufferData buffer(KazBufferHelper::SetUAVTexBuffer(winSize.x, winSize.y, "G-Buffer_Final"));
		RESOURCE_HANDLE view = UavViewHandleMgr::Instance()->GetHandle();

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = winSize.x * winSize.y;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		DescriptorHeapMgr::Instance()->CreateBufferView(view, uavDesc, buffer.bufferWrapper->GetBuffer().Get());
		buffer.bufferWrapper->CreateViewHandle(view);
		buffer.elementNum = winSize.x * winSize.y;
		buffer.rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		buffer.rootParamType = GRAPHICS_PRAMTYPE_DATA3;

		m_gBufferArray.emplace_back(buffer);
	}


	//�N���A����
	{
		m_gBufferClearArray = m_gBufferArray;

		//�ݒ�
		D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		desc.NodeMask = 0;
		m_computeData.desc = desc;
		//�V�F�[�_�[�̃p�X
		m_computeData.shaderData = ShaderOptionData(KazFilePathName::ComputeShaderPath + "ClearGBuffer.hlsl", "CSmain", "cs_6_4", SHADER_TYPE_COMPUTE);

		DispatchComputeShader::DispatchData dispatchData;
		//�f�B�X�p�b�`�̃A�h���X
		dispatchData.x = 1280;
		dispatchData.y = 720;
		dispatchData.z = 1;
		m_computeData.dispatchData = &dispatchData;

		m_gBufferClearArray[ALBEDO].rootParamType = GRAPHICS_PRAMTYPE_DATA;
		m_gBufferClearArray[NORMAL].rootParamType = GRAPHICS_PRAMTYPE_DATA2;

		//�Z�b�g����o�b�t�@
		m_computeData.bufferArray =
		{
			m_gBufferClearArray[ALBEDO],
			m_gBufferClearArray[NORMAL]
		};
	}


}

KazBufferHelper::BufferData GBufferMgr::GetBuffer(BufferType arg_type)
{
	return m_gBufferArray[arg_type];
}

DispatchComputeShader::ComputeData GBufferMgr::ClearData()
{
	return m_computeData;
}
