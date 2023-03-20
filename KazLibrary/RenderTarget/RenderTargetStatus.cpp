#include "RenderTargetStatus.h"
#include"../DirectXCommon/DirectX12Device.h"
#include"../DirectXCommon/DirectX12CmdList.h"
#include"../Buffer/DescriptorHeapMgr.h"
#include<algorithm>

RenderTargetStatus::RenderTargetStatus()
{
	buffers = std::make_unique<CreateGpuBuffer>();

	buffers->handle->SetHandleSize(DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_TEXTURE_RENDERTARGET));
}

RenderTargetStatus::~RenderTargetStatus()
{
}

void RenderTargetStatus::CreateDoubleBuffer(Microsoft::WRL::ComPtr<IDXGISwapChain4> SWAPCHAIN)
{
	//ダブルバッファリング用
	HRESULT result;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	//レンダーターゲットビュー
	heapDesc.NumDescriptors = SWAPCHAIN_MAX_NUM;	//最大二枚
	DirectX12Device::Instance()->dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeaps));


	//裏表の二つ分
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffer(SWAPCHAIN_MAX_NUM);
	for (int i = 0; i < SWAPCHAIN_MAX_NUM; i++)
	{
		//スワップチェーンからバッファを取得
		result = SWAPCHAIN->GetBuffer(i, IID_PPV_ARGS(&backBuffer[i]));
		backBuffer[i]->SetName(L"RenderTarget");

		//ディスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE lHandle = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
		backBuffers.push_back(backBuffer[i]);

		//裏か表かでアドレスがずれる
		lHandle.ptr += i * DirectX12Device::Instance()->dev->GetDescriptorHandleIncrementSize(heapDesc.Type);

		//レンダーターゲットビューの生成
		DirectX12Device::Instance()->dev->CreateRenderTargetView(
			backBuffers[i].Get(),
			nullptr,
			lHandle
		);
	}

	copySwapchain = SWAPCHAIN.Get();
	copyBuffer = backBuffers[0].Get();

	handle = gDepth.CreateBuffer();
	handle2 = gDepth.CreateBuffer();



	//RenderTarget用のヒープ
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;	//レンダーターゲットビュー
	heapDesc.NumDescriptors = 1000;
	//マルチパスレンダリング用のデスクリプタヒープ生成
	DirectX12Device::Instance()->dev->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(multiPassRTVHeap.ReleaseAndGetAddressOf())
	);
}

void RenderTargetStatus::SetDoubleBufferFlame()
{
	bbIndex = copySwapchain->GetCurrentBackBufferIndex();
	//バリア切り替え
	ChangeBarrier(backBuffers[bbIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	gDepth.Clear(handle);
	//レンダータゲットの設定
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH;
	rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIndex * DirectX12Device::Instance()->dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
	DirectX12CmdList::Instance()->cmdList->OMSetRenderTargets(1, &rtvH, false, &gDepth.dsvH[handle]);
}

void RenderTargetStatus::ClearDoubuleBuffer(DirectX::XMFLOAT3 COLOR)
{
	//レンダータゲットのクリア
	float ClearColor[] = { COLOR.x / 255.0f,COLOR.y / 255.0f,COLOR.z / 255.0f, 1.0f };
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH;
	rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIndex * DirectX12Device::Instance()->dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
	DirectX12CmdList::Instance()->cmdList->ClearRenderTargetView(rtvH, ClearColor, 0, nullptr);

	CD3DX12_RECT rect(0, 0, static_cast<long>(WIN_X), static_cast<long>(WIN_Y));
	CD3DX12_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(WIN_X), static_cast<float>(WIN_Y));
	DirectX12CmdList::Instance()->cmdList->RSSetViewports(1, &viewport);
	DirectX12CmdList::Instance()->cmdList->RSSetScissorRects(1, &rect);
}

void RenderTargetStatus::SwapResourceBarrier()
{
	ChangeBarrier(backBuffers[bbIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}

void RenderTargetStatus::PrepareToChangeBarrier(RESOURCE_HANDLE OPEN_RENDERTARGET_HANDLE, RESOURCE_HANDLE CLOSE_RENDERTARGET_HANDLE)
{
	std::vector<RESOURCE_HANDLE> openRendertargetPass = CountPass(OPEN_RENDERTARGET_HANDLE);


	if (CLOSE_RENDERTARGET_HANDLE == -1)
	{
		//ダブルバッファリング用のバリアを閉じる
		ChangeBarrier(backBuffers[bbIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	}
	else
	{
		std::vector<RESOURCE_HANDLE> closeRendertargetPass = CountPass(CLOSE_RENDERTARGET_HANDLE);

		for (int i = 0; i < closeRendertargetPass.size(); ++i)
		{
			//指定のレンダータゲットを閉じる
			ChangeBarrier(buffers->GetBufferData(closeRendertargetPass[i]).Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}
	}


	for (int i = 0; i < openRendertargetPass.size(); ++i)
	{
		//指定のレンダータゲットでバリアをあける
		ChangeBarrier(buffers->GetBufferData(openRendertargetPass[i]).Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}


	//レンダータゲットの設定
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>rtvHs;
	for (int i = 0; i < openRendertargetPass.size(); ++i)
	{
		UINT lHandle = static_cast<UINT>(buffers->handle->CaluNowHandle(openRendertargetPass[i]));
		D3D12_CPU_DESCRIPTOR_HANDLE rtvH = multiPassRTVHeap->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += DirectX12Device::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * lHandle;
		rtvHs.push_back(rtvH);
	}

	DirectX12CmdList::Instance()->cmdList->OMSetRenderTargets(static_cast<UINT>(openRendertargetPass.size()), rtvHs.data(), false, &gDepth.dsvH[handle]);
}

void RenderTargetStatus::PrepareToCloseBarrier(RESOURCE_HANDLE RENDERTARGET_HANDLE)
{
	std::vector<RESOURCE_HANDLE>  closeRendertargetPass = CountPass(RENDERTARGET_HANDLE);
	for (int i = 0; i < closeRendertargetPass.size(); ++i)
	{
		ChangeBarrier(buffers->GetBufferData(closeRendertargetPass[i]).Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	}
}

void RenderTargetStatus::ClearRenderTarget(RESOURCE_HANDLE RENDERTARGET_HANDLE)
{
	//レンダータゲットのクリア
	gDepth.Clear(handle);
	//レンダータゲットの設定

	std::vector<RESOURCE_HANDLE> openHandle = CountPass(RENDERTARGET_HANDLE);
	for (int i = 0; i < openHandle.size(); ++i)
	{
		unsigned int lHandle = static_cast<unsigned int>(buffers->handle->CaluNowHandle(openHandle[i]));
		D3D12_CPU_DESCRIPTOR_HANDLE rtvH = multiPassRTVHeap->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += DirectX12Device::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * lHandle;

		RESOURCE_HANDLE clearHandle = buffers->handle->CaluNowHandle(openHandle[i]);
		float ClearColor[] = { clearColors[clearHandle].x,clearColors[clearHandle].y ,clearColors[clearHandle].z ,clearColors[clearHandle].w };
		DirectX12CmdList::Instance()->cmdList->ClearRenderTargetView(rtvH, ClearColor, 0, nullptr);

		DirectX::XMUINT2 graphSize =
		{
			static_cast<uint32_t>(buffers->GetBufferData(openHandle[i])->GetDesc().Width),
			static_cast<uint32_t>(buffers->GetBufferData(openHandle[i])->GetDesc().Height)
		};
		CD3DX12_RECT rect(0, 0, graphSize.x, graphSize.y);
		CD3DX12_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(graphSize.x), static_cast<float>(graphSize.y));
		DirectX12CmdList::Instance()->cmdList->RSSetViewports(static_cast<UINT>(openHandle.size()), &viewport);
		DirectX12CmdList::Instance()->cmdList->RSSetScissorRects(static_cast<UINT>(openHandle.size()), &rect);
	}
}

RESOURCE_HANDLE RenderTargetStatus::CreateRenderTarget(const KazMath::Vec2<UINT> &GRAPH_SIZE, const DirectX::XMFLOAT3 &CLEAR_COLOR, const DXGI_FORMAT &FORMAT)
{
	//ビューの生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = FORMAT;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;



	D3D12_RESOURCE_DESC resource = RenderTargetStatus::Instance()->copyBuffer->GetDesc();
	resource.Height = GRAPH_SIZE.y;
	resource.Width = static_cast<UINT64>(GRAPH_SIZE.x);
	resource.Format = FORMAT;
	float clearValue[] = { CLEAR_COLOR.x,CLEAR_COLOR.y ,CLEAR_COLOR.z,1.0f };
	D3D12_CLEAR_VALUE clearColor = CD3DX12_CLEAR_VALUE(FORMAT, clearValue);



	KazBufferHelper::BufferResourceData data
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		resource,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearColor,
		"ShaderResourceRenderTarget"
	);
	RESOURCE_HANDLE lHandle = buffers->CreateBuffer(data);


	DescriptorHeapMgr::Instance()->CreateBufferView(lHandle, srvDesc, buffers->GetBufferData(lHandle).Get());
	clearColors[buffers->handle->CaluNowHandle(lHandle)] = { clearValue[0],clearValue[1],clearValue[2],clearValue[3] };


	multiPassRTVHanlde = multiPassRTVHeap->GetCPUDescriptorHandleForHeapStart();
	for (int i = 0; i < buffers->handle->CaluNowHandle(lHandle); i++)
	{
		multiPassRTVHanlde.ptr
			+= DirectX12Device::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	//RTVの作成
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = FORMAT;
	//SRを使ってRTVの生成
	DirectX12Device::Instance()->dev->CreateRenderTargetView(
		buffers->GetBufferData(lHandle).Get(),
		&rtvDesc,
		multiPassRTVHanlde
	);

	std::vector<RESOURCE_HANDLE> lHandles;
	lHandles.push_back(lHandle);
	renderTargetData[buffers->handle->CaluNowHandle(lHandle)] = lHandles;
	return lHandle;
}

std::vector<RESOURCE_HANDLE> RenderTargetStatus::CreateMultiRenderTarget(const std::vector<MultiRenderTargetData> &MULTIRENDER_TARGET_DATA, const DXGI_FORMAT &FORMAT)
{
	//ビューの生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = FORMAT;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	std::vector<RESOURCE_HANDLE> handles;

	RESOURCE_HANDLE buffNum = -1;
	for (int i = 0; i < MULTIRENDER_TARGET_DATA.size(); ++i)
	{
		D3D12_RESOURCE_DESC resource = RenderTargetStatus::Instance()->copyBuffer->GetDesc();
		resource.Height = MULTIRENDER_TARGET_DATA[i].graphSize.y;
		resource.Width = static_cast<UINT64>(MULTIRENDER_TARGET_DATA[i].graphSize.x);
		resource.Format = FORMAT;
		float clearValue[] = { MULTIRENDER_TARGET_DATA[i].backGroundColor.x,MULTIRENDER_TARGET_DATA[i].backGroundColor.y ,MULTIRENDER_TARGET_DATA[i].backGroundColor.z,1.0f };
		D3D12_CLEAR_VALUE clearColor = CD3DX12_CLEAR_VALUE(FORMAT, clearValue);



		KazBufferHelper::BufferResourceData data
		(
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			resource,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearColor,
			"ShaderResourceRenderTarget"
		);
		buffNum = buffers->CreateBuffer(data);


		//DescriptorHeapMgr::Instance()->CreateBufferView(num, rtvDesc, buffers->GetBufferData(num).Get());
		DescriptorHeapMgr::Instance()->CreateBufferView(buffNum, srvDesc, buffers->GetBufferData(buffNum).Get());
		clearColors[buffers->handle->CaluNowHandle(buffNum)] = { clearValue[0],clearValue[1],clearValue[2],clearValue[3] };


		multiPassRTVHanlde = multiPassRTVHeap->GetCPUDescriptorHandleForHeapStart();
		for (int bufferNum = 0; bufferNum < buffers->handle->CaluNowHandle(buffNum); bufferNum++)
		{
			multiPassRTVHanlde.ptr
				+= DirectX12Device::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}

		//RTVの作成
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Format = FORMAT;
		//SRを使ってRTVの生成
		DirectX12Device::Instance()->dev->CreateRenderTargetView(
			buffers->GetBufferData(buffNum).Get(),
			&rtvDesc,
			multiPassRTVHanlde
		);


		handles.push_back(buffNum);
	}

	renderTargetData[buffers->handle->CaluNowHandle(buffNum)] = handles;
	return handles;
}

ID3D12Resource *RenderTargetStatus::GetBufferData(RESOURCE_HANDLE HANDLE)const
{
	return buffers->GetBufferData(HANDLE).Get();
}

D3D12_GPU_DESCRIPTOR_HANDLE RenderTargetStatus::GetViewData(RESOURCE_HANDLE HANDLE)
{
	return DescriptorHeapMgr::Instance()->GetGpuDescriptorView(HANDLE);
}

void RenderTargetStatus::DeleteRenderTarget(RESOURCE_HANDLE HANDLE)
{
	buffers->ReleaseBuffer(HANDLE);
	DescriptorHeapMgr::Instance()->Release(HANDLE);

	for (int i = 0; i < renderTargetData.size(); ++i)
	{
		for (int renderTargetNum = 0; renderTargetNum < renderTargetData[i].size(); ++renderTargetNum)
		{
			if (renderTargetData[i][renderTargetNum] == HANDLE)
			{
				renderTargetData[i].clear();
				renderTargetData[i].shrink_to_fit();
				return;
			}
		}
	}

}

void RenderTargetStatus::DeleteMultiRenderTarget(const std::vector<RESOURCE_HANDLE> &HANDLE)
{
	for (int i = 0; i < HANDLE.size(); ++i)
	{
		buffers->ReleaseBuffer(HANDLE[i]);
		DescriptorHeapMgr::Instance()->Release(HANDLE[i]);
	}


	for (int i = 0; i < renderTargetData.size(); ++i)
	{
		for (int renderTargetNum = 0; renderTargetNum < renderTargetData[i].size(); ++renderTargetNum)
		{
			if (renderTargetData[i][renderTargetNum] == HANDLE[0])
			{
				renderTargetData[i].clear();
				renderTargetData[i].shrink_to_fit();
				return;
			}
		}
	}
}

void RenderTargetStatus::ChangeBarrier(ID3D12Resource *RESOURCE, const D3D12_RESOURCE_STATES &BEFORE_STATE, const D3D12_RESOURCE_STATES &AFTER_STATE)
{
	D3D12_RESOURCE_BARRIER barrier =
		CD3DX12_RESOURCE_BARRIER::Transition(
			RESOURCE,
			BEFORE_STATE,
			AFTER_STATE
		);
	DirectX12CmdList::Instance()->cmdList->ResourceBarrier(1, &barrier);
}

std::vector<RESOURCE_HANDLE> RenderTargetStatus::CountPass(RESOURCE_HANDLE HANDLE)
{
	for (int handleNum = 0; handleNum < renderTargetData.size(); ++handleNum)
	{
		for (int renderTargetNum = 0; renderTargetNum < renderTargetData[handleNum].size(); ++renderTargetNum)
		{
			if (renderTargetData[handleNum][renderTargetNum] == HANDLE)
			{
				return renderTargetData[handleNum];
			}
		}
	}
	assert(0);
	return std::vector<RESOURCE_HANDLE>();
}
