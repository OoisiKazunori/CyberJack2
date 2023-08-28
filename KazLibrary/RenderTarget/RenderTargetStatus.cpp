#include "RenderTargetStatus.h"
#include"../DirectXCommon/DirectX12Device.h"
#include"../DirectXCommon/DirectX12CmdList.h"
#include"../Buffer/DescriptorHeapMgr.h"
#include<algorithm>

RenderTargetStatus::RenderTargetStatus()
{
	renderTargetHandle.SetHandleSize(DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_TEXTURE_RENDERTARGET));
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
		backBuffers.emplace_back(backBuffer[i]);

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


	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NumDescriptors = 1000;
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
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIndex * DirectX12Device::Instance()->dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
	DirectX12CmdList::Instance()->cmdList->OMSetRenderTargets(1, &rtvH, false, &gDepth.dsvH[handle]);
}

void RenderTargetStatus::ClearDoubuleBuffer(DirectX::XMFLOAT3 COLOR)
{
	//レンダータゲットのクリア
	float clearColor[] = { COLOR.x / 255.0f,COLOR.y / 255.0f,COLOR.z / 255.0f, 0.0f };
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = rtvHeaps->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += bbIndex * DirectX12Device::Instance()->dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
	DirectX12CmdList::Instance()->cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

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
			ChangeBarrier(buffers[i].bufferWrapper->GetBuffer(closeRendertargetPass[i]).Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		}
	}


	for (int i = 0; i < openRendertargetPass.size(); ++i)
	{
		//指定のレンダータゲットでバリアをあける
		ChangeBarrier(buffers[i].bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
	}


	//レンダータゲットの設定
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>rtvHs;
	for (int i = 0; i < openRendertargetPass.size(); ++i)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvH = GetRTVHandle(openRendertargetPass[i]);
		rtvHs.emplace_back(rtvH);
	}

	DirectX12CmdList::Instance()->cmdList->OMSetRenderTargets(static_cast<UINT>(openRendertargetPass.size()), rtvHs.data(), false, &gDepth.dsvH[handle]);
}

void RenderTargetStatus::PrepareToCloseBarrier(RESOURCE_HANDLE RENDERTARGET_HANDLE)
{
	std::vector<RESOURCE_HANDLE> closeRendertargetPass = CountPass(RENDERTARGET_HANDLE);
	for (int i = 0; i < closeRendertargetPass.size(); ++i)
	{
		unsigned int handle = static_cast<unsigned int>(renderTargetHandle.CaluNowHandle(closeRendertargetPass[i]));
		ChangeBarrier(buffers[handle].bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
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
		unsigned int handle = static_cast<unsigned int>(renderTargetHandle.CaluNowHandle(openHandle[i]));
		D3D12_CPU_DESCRIPTOR_HANDLE rtvH = GetRTVHandle(openHandle[i]);

		float ClearColor[] = { clearColors[handle].x,clearColors[handle].y ,clearColors[handle].z ,clearColors[handle].w };
		DirectX12CmdList::Instance()->cmdList->ClearRenderTargetView(rtvH, ClearColor, 0, nullptr);

		DirectX::XMUINT2 graphSize =
		{
			static_cast<uint32_t>(buffers[handle].bufferWrapper->GetBuffer()->GetDesc().Width),
			static_cast<uint32_t>(buffers[handle].bufferWrapper->GetBuffer()->GetDesc().Height)
		};
		CD3DX12_RECT rect(0, 0, graphSize.x, graphSize.y);
		CD3DX12_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(graphSize.x), static_cast<float>(graphSize.y));
		DirectX12CmdList::Instance()->cmdList->RSSetViewports(static_cast<UINT>(openHandle.size()), &viewport);
		DirectX12CmdList::Instance()->cmdList->RSSetScissorRects(1, &rect);
	}
}

void RenderTargetStatus::SetDepth(RESOURCE_HANDLE RENDERTARGET_HANDLE)
{
	if (RENDERTARGET_HANDLE == -1)
	{
		return;
	}
	//レンダータゲットの設定
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = GetRTVHandle(RENDERTARGET_HANDLE);
	DirectX12CmdList::Instance()->cmdList->OMSetRenderTargets(1, &rtvH, false, &gDepth.dsvH[handle]);
}

RESOURCE_HANDLE RenderTargetStatus::CreateRenderTarget(const KazMath::Vec2<UINT> &GRAPH_SIZE, const DirectX::XMFLOAT3 &CLEAR_COLOR, const DXGI_FORMAT &FORMAT)
{
	//レンダーターゲット用のバッファ生成準備
	MultiRenderTargetData bufferData;
	bufferData.backGroundColor = { CLEAR_COLOR.x / 255.0f,CLEAR_COLOR.y / 255.0f,CLEAR_COLOR.z / 255.0f };
	bufferData.format = FORMAT;
	bufferData.graphSize = GRAPH_SIZE;
	//生成
	RESOURCE_HANDLE handle = GenerateRenderTargetBuffer(bufferData);
	//ハンドル格納
	renderTargetData[renderTargetHandle.CaluNowHandle(handle)].emplace_back(handle);

	return handle;
}

std::vector<RESOURCE_HANDLE> RenderTargetStatus::CreateMultiRenderTarget(const std::vector<MultiRenderTargetData> &MULTIRENDER_TARGET_DATA, const DXGI_FORMAT &FORMAT)
{
	std::vector<RESOURCE_HANDLE> handles;
	for (int i = 0; i < MULTIRENDER_TARGET_DATA.size(); ++i)
	{
		MultiRenderTargetData bufferData = MULTIRENDER_TARGET_DATA[i];
		bufferData.format = FORMAT;
		RESOURCE_HANDLE handle = GenerateRenderTargetBuffer(bufferData);
		handles.emplace_back(handle);
	}
	renderTargetData[renderTargetHandle.CaluNowHandle(handles.front())] = handles;
	return handles;
}

std::vector<RESOURCE_HANDLE> RenderTargetStatus::CreateMultiRenderTarget(const std::vector<MultiRenderTargetData> &MULTIRENDER_TARGET_DATA)
{
	std::vector<RESOURCE_HANDLE> handles;
	for (int i = 0; i < MULTIRENDER_TARGET_DATA.size(); ++i)
	{
		RESOURCE_HANDLE handle = GenerateRenderTargetBuffer(MULTIRENDER_TARGET_DATA[i]);
		handles.emplace_back(handle);
	}
	renderTargetData[renderTargetHandle.CaluNowHandle(handles.front())] = handles;
	return handles;
}

ID3D12Resource *RenderTargetStatus::GetBufferData(RESOURCE_HANDLE HANDLE)const
{
	return buffers[HANDLE].bufferWrapper->GetBuffer().Get();
}

const KazBufferHelper::BufferData &RenderTargetStatus::GetBuffer(RESOURCE_HANDLE HANDLE)
{
	return buffers[renderTargetHandle.CaluNowHandle(HANDLE)];
}

D3D12_GPU_DESCRIPTOR_HANDLE RenderTargetStatus::GetViewData(RESOURCE_HANDLE HANDLE)
{
	return DescriptorHeapMgr::Instance()->GetGpuDescriptorView(HANDLE);
}

void RenderTargetStatus::DeleteRenderTarget(RESOURCE_HANDLE HANDLE)
{
	buffers[HANDLE].bufferWrapper->Release();
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
		buffers[HANDLE[i]].bufferWrapper->Release();
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

RESOURCE_HANDLE RenderTargetStatus::GenerateRenderTargetBuffer(const MultiRenderTargetData &arg_renderTargetBufferData)
{
	//ビューの生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = arg_renderTargetBufferData.format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;



	D3D12_RESOURCE_DESC resource = RenderTargetStatus::Instance()->copyBuffer->GetDesc();
	resource.Height = arg_renderTargetBufferData.graphSize.y;
	resource.Width = static_cast<UINT64>(arg_renderTargetBufferData.graphSize.x);
	resource.Format = arg_renderTargetBufferData.format;
	float clearValue[] = { arg_renderTargetBufferData.backGroundColor.x / 255.0f,arg_renderTargetBufferData.backGroundColor.y / 255.0f ,arg_renderTargetBufferData.backGroundColor.z / 255.0f,1.0f };
	D3D12_CLEAR_VALUE clearColor = CD3DX12_CLEAR_VALUE(resource.Format, clearValue);



	KazBufferHelper::BufferResourceData data
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		resource,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearColor,
		"ShaderResourceRenderTarget"
	);
	buffers.emplace_back(data);

	RESOURCE_HANDLE descriptorHandle = renderTargetHandle.GetHandle();
	RESOURCE_HANDLE handle = renderTargetHandle.CaluNowHandle(descriptorHandle);

	DescriptorHeapMgr::Instance()->CreateBufferView(descriptorHandle, srvDesc, buffers[handle].bufferWrapper->GetBuffer().Get());
	clearColors[handle] = { clearValue[0],clearValue[1],clearValue[2],clearValue[3] };

	D3D12_CPU_DESCRIPTOR_HANDLE multiPassRTVHanlde = multiPassRTVHeap->GetCPUDescriptorHandleForHeapStart();
	multiPassRTVHanlde.ptr += DirectX12Device::Instance()->dev->GetDescriptorHandleIncrementSize(heapDesc.Type) * handle;

	//RTVの作成
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = resource.Format;
	//SRを使ってRTVの生成
	DirectX12Device::Instance()->dev->CreateRenderTargetView(
		buffers[handle].bufferWrapper->GetBuffer().Get(),
		&rtvDesc,
		multiPassRTVHanlde
	);

	buffers[handle].bufferWrapper->CreateViewHandle(descriptorHandle);

	buffers[handle].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;

	return descriptorHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetStatus::GetRTVHandle(RESOURCE_HANDLE arg_handle)
{
	UINT lHandle = static_cast<UINT>(renderTargetHandle.CaluNowHandle(arg_handle));
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = multiPassRTVHeap->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += DirectX12Device::Instance()->dev->GetDescriptorHandleIncrementSize(heapDesc.Type) * lHandle;
	return rtvH;
}
