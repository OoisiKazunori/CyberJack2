#include "GBufferMgr.h"
#include"../KazLibrary/Buffer/UavViewHandleMgr.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"

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

}

KazBufferHelper::BufferData GBufferMgr::GetBuffer(BufferType arg_type)
{
	return m_gBufferArray[arg_type];
}
