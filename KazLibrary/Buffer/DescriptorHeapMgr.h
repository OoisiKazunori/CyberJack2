#pragma once
#include"../DirectXCommon/Base.h"
#include"../Helper/ISinglton.h"
#include"../Helper/KazHelper.h"
#include"../Helper/KazBufferHelper.h"
#include"../Buffer/CreateGpuBuffer.h"

class DescriptorHeapMgr :public ISingleton<DescriptorHeapMgr>
{
public:
	DescriptorHeapMgr();
	~DescriptorHeapMgr();


	void CreateBufferView(RESOURCE_HANDLE HANDLE, const D3D12_CONSTANT_BUFFER_VIEW_DESC &BUFFER_VIEW);
	void CreateBufferView(RESOURCE_HANDLE HANDLE, const D3D12_SHADER_RESOURCE_VIEW_DESC &BUFFER_VIEW, ID3D12Resource *ADDRESS);
	void CreateBufferView(RESOURCE_HANDLE HANDLE, const D3D12_RENDER_TARGET_VIEW_DESC &BUFFER_VIEW, ID3D12Resource *ADDRESS);
	void CreateBufferView(RESOURCE_HANDLE HANDLE, const D3D12_UNORDERED_ACCESS_VIEW_DESC &BUFFER_VIEW, ID3D12Resource *ADDRESS, ID3D12Resource *COUNTER_RESOURCE = nullptr);
	void Release(RESOURCE_HANDLE HANDLE);


	BufferMemorySize GetSize(const BufferMemory &TYPE)const;
	const D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorView(RESOURCE_HANDLE HANDLE);
	const D3D12_CPU_DESCRIPTOR_HANDLE GetCpuDescriptorView(RESOURCE_HANDLE HANDLE);
	void SetDescriptorHeap();


	int GetType(RESOURCE_HANDLE HANDLE);

private:

	//デスクリプタヒープの処理-----------------------------------------------------------------------------
	static const short DESCRIPTOR_HEAP_SIZE_MAX = 4096;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heaps;
	std::array<BufferMemorySize, DESCRIPTORHEAP_MEMORY_MAX> heapMaxSizes;
	UINT64 shaderResourceHeapIncreSize;
	UINT64 rtvHeapIncreSize;
	std::array<bool, DESCRIPTOR_HEAP_SIZE_MAX> usedHandle;

	bool isSafeToReadThisHandle(RESOURCE_HANDLE HANDLE)
	{
		return KazHelper::IsitInAnArray(HANDLE, DESCRIPTOR_HEAP_SIZE_MAX) && usedHandle[HANDLE];
	};
	bool isSafeToUseThisHandle(RESOURCE_HANDLE HANDLE)
	{
		return KazHelper::IsitInAnArray(HANDLE, DESCRIPTOR_HEAP_SIZE_MAX) && !usedHandle[HANDLE];
	};
};

