#pragma once
#include"../DirectXCommon/Base.h"
#include"../KazLibrary/DirectXCommon/DirectX12CmdList.h"
#include"../KazLibrary/DirectXCommon/DirectX12Device.h"

struct BufferMemorySize
{
	int startSize;
	int endSize;

	BufferMemorySize() :startSize(-1), endSize(-1)
	{};
};

enum BufferMemory
{
	DESCRIPTORHEAP_MEMORY_NONE = -1,
	DESCRIPTORHEAP_MEMORY_TEXTURE_SPRITE,
	DESCRIPTORHEAP_MEMORY_TEXTURE_OBJ,
	DESCRIPTORHEAP_MEMORY_TEXTURE_FBX,
	DESCRIPTORHEAP_MEMORY_TEXTURE_RENDERTARGET,
	DESCRIPTORHEAP_MEMORY_TEXTURE_COMPUTEBUFFER,
	DESCRIPTORHEAP_MEMORY_CBV,
	DESCRIPTORHEAP_MEMORY_SRV,
	DESCRIPTORHEAP_MEMORY_MOVIE,
	DESCRIPTORHEAP_MEMORY_MAX
};

namespace KazBufferHelper
{
	struct BufferResourceData
	{
		D3D12_HEAP_PROPERTIES heapProperties;
		D3D12_HEAP_FLAGS heapFlags;
		D3D12_RESOURCE_DESC resourceDesc;
		D3D12_RESOURCE_STATES resourceState;
		D3D12_CLEAR_VALUE *clearValue;
		std::string BufferName;

		BufferResourceData
		(
			const D3D12_HEAP_PROPERTIES &HEAP_PROP,
			const D3D12_HEAP_FLAGS &HEAP_FLAGS,
			const D3D12_RESOURCE_DESC &RESOURCE_DESC,
			const D3D12_RESOURCE_STATES &RESOURCE_STATE,
			D3D12_CLEAR_VALUE *CLEAR_VALUE,
			const std::string &BUFFER_NAME
		)
			:heapProperties(HEAP_PROP),
			heapFlags(HEAP_FLAGS),
			resourceDesc(RESOURCE_DESC),
			resourceState(RESOURCE_STATE),
			clearValue(CLEAR_VALUE),
			BufferName(BUFFER_NAME)
		{
		};
	};


	/// <summary>
	/// 定数バッファを生成する際に必要な設定を簡易に纏めた物
	/// </summary>
	/// <param name="BUFFER_SIZE">定数バッファのサイズ</param>
	/// <returns>定数バッファの生成に必要な設定</returns>
	KazBufferHelper::BufferResourceData SetConstBufferData(const unsigned int &BUFFER_SIZE, const std::string &BUFFER_NAME = "ConstBuffer");

	/// <summary>
	/// シェーダーリソースバッファを生成する際に必要な設定を簡易に纏めた物
	/// <param name="TEXTURE_DATA">読み込んだテクスチャの設定</param>
	/// <returns>シェーダーリソースバッファの生成に必要な設定</returns>
	KazBufferHelper::BufferResourceData SetShaderResourceBufferData(const D3D12_RESOURCE_DESC &TEXTURE_DATA, const std::string &BUFFER_NAME = "ShaderResourceBuffer");


	KazBufferHelper::BufferResourceData SetVertexBufferData(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME = "VertexBuffer");


	KazBufferHelper::BufferResourceData SetIndexBufferData(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME = "IndexBuffer");


	KazBufferHelper::BufferResourceData SetRenderTargetData(const D3D12_HEAP_PROPERTIES &HEAP_PROPERTIES, const D3D12_RESOURCE_DESC &RESOURCE, D3D12_CLEAR_VALUE *CLEAR_COLOR, const std::string &BUFFER_NAME = "RenderTarget");

	KazBufferHelper::BufferResourceData SetStructureBuffer(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME = "StructureBuffer");

	KazBufferHelper::BufferResourceData SetRWStructuredBuffer(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME = "RWStructureBuffer");

	KazBufferHelper::BufferResourceData SetOnlyReadStructuredBuffer(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME = "OnlyReadStructureBuffer");


	KazBufferHelper::BufferResourceData SetCommandBufferData(const unsigned int &BUFFER_SIZE, const std::string &BUFFER_NAME = "CommandBuffer");

	D3D12_VERTEX_BUFFER_VIEW SetVertexBufferView(const D3D12_GPU_VIRTUAL_ADDRESS &GPU_ADDRESS, BUFFER_SIZE BUFFER_SIZE, const unsigned int &ONE_VERTICES_SIZE);

	D3D12_INDEX_BUFFER_VIEW SetIndexBufferView(const D3D12_GPU_VIRTUAL_ADDRESS &GPU_ADDRESS, BUFFER_SIZE BUFFER_SIZE);

	D3D12_UNORDERED_ACCESS_VIEW_DESC SetUnorderedAccessView(BUFFER_SIZE STRUCTURE_BYTE_SIZE, UINT NUM_ELEMENTS);

	KazBufferHelper::BufferResourceData SetGPUBufferData(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME = "IndexBuffer");


	template<typename T>
	T GetBufferSize(size_t BUFFER_SIZE, unsigned long long STRUCTURE_SIZE)
	{
		return static_cast<T>(BUFFER_SIZE * static_cast<int>(STRUCTURE_SIZE));
	};


}

