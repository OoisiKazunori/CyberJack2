#include "CreateGpuBuffer.h"
#include"../DirectXCommon/DirectX12Device.h"
#include"../DirectXCommon/DirectX12CmdList.h"
#include"../Helper/KazHelper.h"

CreateGpuBuffer::CreateGpuBuffer()
{
	increSize = DirectX12Device::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	handle = std::make_unique<HandleMaker>();
}

CreateGpuBuffer::~CreateGpuBuffer()
{
}

void CreateGpuBuffer::CreateDescriptorHeap(unsigned int DESCRIPTOR_NUM_MAX)
{
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	//シェーダーから見える
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	//バッファの数
	descHeapDesc.NumDescriptors = DESCRIPTOR_NUM_MAX;
	DirectX12Device::Instance()->dev->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&heap));
}

RESOURCE_HANDLE CreateGpuBuffer::CreateBuffer(D3D12_HEAP_PROPERTIES HEAP_PROPERTIES, D3D12_RESOURCE_DESC RESOURCE, D3D12_RESOURCE_STATES RESOURCE_STATE, D3D12_CLEAR_VALUE *CLEAR_VALUE)
{
	HRESULT result;

	//ハンドルの取得
	RESOURCE_HANDLE lHandle = handle->GetHandle();
	RESOURCE_HANDLE lCaluHandle = handle->CaluNowHandle(lHandle);


	//バッファの生成
	result = DirectX12Device::Instance()->dev->CreateCommittedResource(
		&HEAP_PROPERTIES,
		D3D12_HEAP_FLAG_NONE,
		&RESOURCE,
		RESOURCE_STATE,
		CLEAR_VALUE,
		//無駄なバッファの確保をしないように、現在のハンドル数値 - ハンドルの始めの数値で添え字は0から始めるようにする
		IID_PPV_ARGS(&buffers[lCaluHandle])
	);

	//失敗したら-1を返す
	if (result != S_OK)
	{
		handle->DeleteHandle(lHandle);
		return -1;
	}

	buffers[lCaluHandle]->SetName(L"CreateGpuBuffer");
	return lHandle;
}

RESOURCE_HANDLE CreateGpuBuffer::CreateBuffer(const KazBufferHelper::BufferResourceData &BUFFER_OPTION)
{
	HRESULT result;

	//ハンドルの取得
	RESOURCE_HANDLE lHandle = handle->GetHandle();
	RESOURCE_HANDLE lCaluHandle = handle->CaluNowHandle(lHandle);

	//バッファの生成
	result = DirectX12Device::Instance()->dev->CreateCommittedResource(
		&BUFFER_OPTION.heapProperties,
		BUFFER_OPTION.heapFlags,
		&BUFFER_OPTION.resourceDesc,
		BUFFER_OPTION.resourceState,
		BUFFER_OPTION.clearValue,
		IID_PPV_ARGS(&buffers[lCaluHandle])
	);

	//失敗したら-1を返す
	if (result != S_OK)
	{
		handle->DeleteHandle(lHandle);
		return -1;
	}

	const unsigned int BUFFER_SIZE = 256;
	std::array<wchar_t, BUFFER_SIZE> string;
	KazHelper::ConvertStringToWchar_t(BUFFER_OPTION.BufferName, string.data(), BUFFER_SIZE);
	buffers[lCaluHandle]->SetName(string.data());
	return lHandle;
}

void CreateGpuBuffer::TransData(RESOURCE_HANDLE HANDLE, void *DATA, const unsigned int &DATA_SIZE)
{
	RESOURCE_HANDLE lCaluHandle = handle->CaluNowHandle(HANDLE);
	void *dataMap = nullptr;
	auto result = buffers[lCaluHandle]->Map(0, nullptr, (void **)&dataMap);
	if (SUCCEEDED(result))
	{
		memcpy(dataMap, DATA, DATA_SIZE);
		buffers[lCaluHandle]->Unmap(0, nullptr);
	}
}

void CreateGpuBuffer::CreateBufferView(RESOURCE_HANDLE HANDLE, D3D12_CONSTANT_BUFFER_VIEW_DESC VIEW)
{
	//ビューの作成
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = VIEW;
	heapHandle = heap->GetCPUDescriptorHandleForHeapStart();
	for (int i = 0; i < HANDLE; i++)
	{
		heapHandle.ptr += increSize;
	}
	DirectX12Device::Instance()->dev->CreateConstantBufferView(&cbvDesc, heapHandle);
}

void CreateGpuBuffer::CreateBufferView(RESOURCE_HANDLE HANDLE, D3D12_SHADER_RESOURCE_VIEW_DESC VIEW)
{
	RESOURCE_HANDLE lCaluHandle = handle->CaluNowHandle(HANDLE);
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = VIEW;
	heapHandle = heap->GetCPUDescriptorHandleForHeapStart();
	for (int i = 0; i < HANDLE; i++)
	{
		heapHandle.ptr += increSize;
	}
	DirectX12Device::Instance()->dev->CreateShaderResourceView(buffers[lCaluHandle].Get(), &srvDesc, heapHandle);
}

void CreateGpuBuffer::ReleaseBuffer(RESOURCE_HANDLE HANDLE)
{
	RESOURCE_HANDLE lCaluHandle = handle->CaluNowHandle(HANDLE);
	if (KazHelper::IsitInAnArray(lCaluHandle, buffers.size()))
	{
		buffers[lCaluHandle].Reset();
		handle->DeleteHandle(HANDLE);
	}
}

D3D12_GPU_DESCRIPTOR_HANDLE CreateGpuBuffer::GetViewPointer(RESOURCE_HANDLE HANDLE)
{
	//指定のビューを探す
	ID3D12DescriptorHeap *ppHeap[] = { heap.Get() };
	DirectX12CmdList::Instance()->cmdList->SetDescriptorHeaps(_countof(ppHeap), ppHeap);

	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleCBV;
	gpuDescHandleCBV = heap->GetGPUDescriptorHandleForHeapStart();;
	gpuDescHandleCBV.ptr += increSize * HANDLE;

	return gpuDescHandleCBV;
}

Microsoft::WRL::ComPtr<ID3D12Resource> CreateGpuBuffer::GetBufferData(RESOURCE_HANDLE HANDLE)
{
	RESOURCE_HANDLE lCaluHandle = handle->CaluNowHandle(HANDLE);
	if (KazHelper::IsitInAnArray(lCaluHandle, buffers.size()))
	{
		return buffers[lCaluHandle];
	}
	else
	{
		return nullptr;
	}
}

D3D12_GPU_VIRTUAL_ADDRESS CreateGpuBuffer::GetGpuAddress(RESOURCE_HANDLE HANDLE)
{
	RESOURCE_HANDLE lCaluHandle = handle->CaluNowHandle(HANDLE);
	if (KazHelper::IsitInAnArray(lCaluHandle, buffers.size()))
	{
		return buffers[lCaluHandle]->GetGPUVirtualAddress();
	}

	return static_cast<UINT64>(-1);
}

void *CreateGpuBuffer::GetMapAddres(RESOURCE_HANDLE HANDLE)
{
	void *dataMap = nullptr;
	buffers[HANDLE]->Map(0, nullptr, (void **)&dataMap);
	return dataMap;
}