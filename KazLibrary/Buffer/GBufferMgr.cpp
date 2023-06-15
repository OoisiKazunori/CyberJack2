#include "GBufferMgr.h"
#include"../KazLibrary/Buffer/UavViewHandleMgr.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"RenderTarget/RenderTargetStatus.h"

//ワールド座標、ラフネス、メタルネス、スぺキュラ、オブジェクトが反射するか屈折するか(インデックス)、Albedo、法線、カメラ座標(定数バッファでも可能)
GBufferMgr::GBufferMgr()
{
	KazMath::Vec2<UINT>winSize(1280, 720);

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


	//クリア処理
	{
		m_gBufferClearArray = m_gBufferArray;

		//設定
		D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		desc.NodeMask = 0;
		m_computeData.desc = desc;
		//シェーダーのパス
		m_computeData.shaderData = ShaderOptionData(KazFilePathName::ComputeShaderPath + "ClearGBuffer.hlsl", "CSmain", "cs_6_4", SHADER_TYPE_COMPUTE);

		DispatchComputeShader::DispatchData dispatchData;
		//ディスパッチのアドレス
		dispatchData.x = 1280;
		dispatchData.y = 720;
		dispatchData.z = 1;
		m_computeData.dispatchData = &dispatchData;

		m_gBufferClearArray[ALBEDO].rootParamType = GRAPHICS_PRAMTYPE_DATA;
		m_gBufferClearArray[NORMAL].rootParamType = GRAPHICS_PRAMTYPE_DATA2;

		//セットするバッファ
		m_computeData.bufferArray =
		{
			m_gBufferClearArray[ALBEDO],
			m_gBufferClearArray[NORMAL]
		};
	}


	//G-Buffer用のレンダーターゲット生成
	{
		std::vector<MultiRenderTargetData> multiRenderTargetArray(2);
		//アルベド
		multiRenderTargetArray[ALBEDO].backGroundColor = { 0.0f,0.0f,0.0f };
		multiRenderTargetArray[ALBEDO].graphSize = winSize;
		multiRenderTargetArray[ALBEDO].format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//ノーマル
		multiRenderTargetArray[NORMAL].backGroundColor = { 0.0f,0.0f,0.0f };
		multiRenderTargetArray[NORMAL].graphSize = winSize;
		multiRenderTargetArray[NORMAL].format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		m_gBufferRenderTargetHandleArray = RenderTargetStatus::Instance()->CreateMultiRenderTarget(multiRenderTargetArray);

		//ラフネス、メタルネス、スぺキュラ、屈折判定(0...反射しない、1...反射する、2...屈折する)
		//multiRenderTargetArray[2].backGroundColor = BG_COLOR;
		//multiRenderTargetArray[2].graphSize = winSize;
		//multiRenderTargetArray[2].format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	}

}

KazBufferHelper::BufferData GBufferMgr::GetBuffer(BufferType arg_type)
{
	return m_gBufferArray[arg_type];
}

std::vector<RESOURCE_HANDLE> GBufferMgr::GetRenderTarget()
{
	return m_gBufferRenderTargetHandleArray;
}

DispatchComputeShader::ComputeData GBufferMgr::ClearData()
{
	return m_computeData;
}
