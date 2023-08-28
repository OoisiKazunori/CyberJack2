#include "KazBufferHelper.h"
#include"../KazLibrary/Buffer/UavViewHandleMgr.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"

KazBufferHelper::BufferResourceData KazBufferHelper::SetConstBufferData(const unsigned int &BUFFER_SIZE, const std::string &BUFFER_NAME)
{
	BufferResourceData data
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		CD3DX12_RESOURCE_DESC::Buffer((BUFFER_SIZE + 0xff) & ~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		BUFFER_NAME
	);
	return data;
}

KazBufferHelper::BufferResourceData KazBufferHelper::SetShaderResourceBufferData(const D3D12_RESOURCE_DESC &TEXTURE_DATA, const std::string &BUFFER_NAME)
{
	BufferResourceData data
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		TEXTURE_DATA,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		BUFFER_NAME
	);
	return data;
}

KazBufferHelper::BufferResourceData KazBufferHelper::SetVertexBufferData(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME)
{
	BufferResourceData data
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		CD3DX12_RESOURCE_DESC::Buffer(BUFFER_SIZE),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		BUFFER_NAME
	);
	return data;
}
KazBufferHelper::BufferResourceData KazBufferHelper::SetIndexBufferData(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME)
{
	BufferResourceData data
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		CD3DX12_RESOURCE_DESC::Buffer(BUFFER_SIZE),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		BUFFER_NAME
	);
	return data;
}

KazBufferHelper::BufferResourceData KazBufferHelper::SetRenderTargetData(const D3D12_HEAP_PROPERTIES &HEAP_PROPERTIES, const D3D12_RESOURCE_DESC &RESOURCE, D3D12_CLEAR_VALUE *CLEAR_COLOR, const std::string &BUFFER_NAME)
{
	KazBufferHelper::BufferResourceData data
	(
		HEAP_PROPERTIES,
		D3D12_HEAP_FLAG_NONE,
		RESOURCE,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		CLEAR_COLOR,
		BUFFER_NAME
	);
	return data;
}

KazBufferHelper::BufferResourceData KazBufferHelper::SetStructureBuffer(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME)
{
	KazBufferHelper::BufferResourceData data
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		CD3DX12_RESOURCE_DESC::Buffer(BUFFER_SIZE),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		BUFFER_NAME
	);
	return data;
}

KazBufferHelper::BufferResourceData KazBufferHelper::SetRWStructuredBuffer(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME)
{
	D3D12_HEAP_PROPERTIES prop = {};
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	prop.CreationNodeMask = 1;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	prop.Type = D3D12_HEAP_TYPE_CUSTOM;
	prop.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(BUFFER_SIZE, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	KazBufferHelper::BufferResourceData data
	(
		prop,
		D3D12_HEAP_FLAG_NONE,
		desc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		BUFFER_NAME
	);
	return data;
}

KazBufferHelper::BufferResourceData KazBufferHelper::SetUAVTexBuffer(int width, int height, DXGI_FORMAT format, const std::string &BUFFER_NAME)
{
	D3D12_RESOURCE_DESC desc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			format,
			width,
			(UINT)height,
			(UINT16)1,
			(UINT16)1
		);
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

	KazBufferHelper::BufferResourceData data
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		desc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		BUFFER_NAME
	);

	return data;
}

KazBufferHelper::BufferResourceData KazBufferHelper::SetUAV3DTexBuffer(int arg_width, int arg_height, int arg_depth, DXGI_FORMAT arg_format, const std::string& arg_bufferName)
{
	D3D12_RESOURCE_DESC desc{};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
	desc.Alignment = 0;
	desc.Width = arg_width;
	desc.Height = arg_height;
	desc.DepthOrArraySize = arg_depth;
	desc.MipLevels = 1;
	desc.Format = arg_format;
	desc.SampleDesc = { 1, 0 };
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	KazBufferHelper::BufferResourceData data
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		desc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		arg_bufferName
	);

	return data;
}

KazBufferHelper::BufferResourceData KazBufferHelper::SetOnlyReadStructuredBuffer(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME)
{
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(BUFFER_SIZE, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	KazBufferHelper::BufferResourceData data
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		desc,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		nullptr,
		BUFFER_NAME
	);
	return data;
}

KazBufferHelper::BufferResourceData KazBufferHelper::SetCommandBufferData(const unsigned int &BUFFER_SIZE, const std::string &BUFFER_NAME)
{
	D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(BUFFER_SIZE);

	KazBufferHelper::BufferResourceData data
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		BUFFER_NAME
	);

	return data;
}

D3D12_VERTEX_BUFFER_VIEW KazBufferHelper::SetVertexBufferView(const D3D12_GPU_VIRTUAL_ADDRESS &GPU_ADDRESS, BUFFER_SIZE BUFFER_SIZE, const unsigned int &ONE_VERTICES_SIZE)
{
	D3D12_VERTEX_BUFFER_VIEW view;
	view.BufferLocation = GPU_ADDRESS;
	view.SizeInBytes = BUFFER_SIZE;
	view.StrideInBytes = ONE_VERTICES_SIZE;
	return view;
}

D3D12_INDEX_BUFFER_VIEW KazBufferHelper::SetIndexBufferView(const D3D12_GPU_VIRTUAL_ADDRESS &GPU_ADDRESS, BUFFER_SIZE BUFFER_SIZE)
{
	D3D12_INDEX_BUFFER_VIEW view;
	view.BufferLocation = GPU_ADDRESS;
	view.Format = DXGI_FORMAT_R32_UINT;
	view.SizeInBytes = BUFFER_SIZE;
	return view;
}

D3D12_UNORDERED_ACCESS_VIEW_DESC KazBufferHelper::SetUnorderedAccessView(BUFFER_SIZE STRUCTURE_BYTE_SIZE, UINT NUM_ELEMENTS)
{
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = NUM_ELEMENTS;
	uavDesc.Buffer.StructureByteStride = STRUCTURE_BYTE_SIZE;
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	return uavDesc;
}

D3D12_UNORDERED_ACCESS_VIEW_DESC KazBufferHelper::SetUnorderedAccessTextureView(BUFFER_SIZE STRUCTURE_BYTE_SIZE, UINT NUM_ELEMENTS)
{
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = NUM_ELEMENTS;
	uavDesc.Buffer.StructureByteStride = STRUCTURE_BYTE_SIZE;
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	return uavDesc;
}

D3D12_UNORDERED_ACCESS_VIEW_DESC KazBufferHelper::SetUnorderedAccess3DTextureView(BUFFER_SIZE arg_structureByteSize, UINT arg_numElements)
{
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
	uavDesc.Texture3D.MipSlice = 0;
	uavDesc.Texture3D.FirstWSlice = 0;
	uavDesc.Texture3D.WSize = -1;
	return uavDesc;
}

void KazBufferHelper::ID3D12ResourceWrapper::CreateBuffer(const BufferResourceData& BUFFER_OPTION)
{
	for (int i = 0; i < buffer.size(); ++i)
	{
		HRESULT lResult;
		//バッファの生成
		lResult = DirectX12Device::Instance()->dev->CreateCommittedResource(
			&BUFFER_OPTION.heapProperties,
			BUFFER_OPTION.heapFlags,
			&BUFFER_OPTION.resourceDesc,
			BUFFER_OPTION.resourceState,
			BUFFER_OPTION.clearValue,
			IID_PPV_ARGS(&buffer[i])
		);

		assert(lResult == S_OK);

		const unsigned int BUFFER_SIZE = 256;
		std::array<wchar_t, BUFFER_SIZE> string;
		KazHelper::ConvertStringToWchar_t(BUFFER_OPTION.BufferName, string.data(), BUFFER_SIZE);
		buffer[i]->SetName(string.data());
		bufferName = BUFFER_OPTION.BufferName;

		resourceState = BUFFER_OPTION.resourceState;

		if (BUFFER_OPTION.heapProperties.Type != D3D12_HEAP_TYPE_DEFAULT &&
			BUFFER_OPTION.heapProperties.Type != D3D12_HEAP_TYPE_CUSTOM)
		{
			auto result = buffer[i]->Map(0, nullptr, (void**)&bufferMapPtr[i]);
			if (FAILED(result))
			{
				assert(0);
			}
			isVRAMBufferFlag = false;
		}
		else
		{
			isVRAMBufferFlag = true;
			bufferMapPtr[i] = nullptr;
		}
	}

}

void KazBufferHelper::ID3D12ResourceWrapper::TransData(void* DATA, const unsigned int& DATA_SIZE)
{
	if (!isVRAMBufferFlag)
	{
		for (int i = 0; i < buffer.size(); ++i)
		{
			memcpy(bufferMapPtr[i], DATA, DATA_SIZE);
		}
	}
	else
	{
		//VRAMのバッファはCPUから転送できません
		assert(0);
	}
}

void KazBufferHelper::ID3D12ResourceWrapper::TransData(void* DATA, unsigned int START_DATA_SIZE, unsigned int DATA_SIZE)
{
	void* dataMap = nullptr;
	for (int i = 0; i < buffer.size(); ++i)
	{
		auto result = buffer[i]->Map(0, nullptr, (void**)&dataMap);

		void* data = &dataMap + START_DATA_SIZE;
		if (SUCCEEDED(result))
		{
			memcpy(data, DATA, DATA_SIZE);
			buffer[i]->Unmap(0, nullptr);
		}
	}
}

void KazBufferHelper::ID3D12ResourceWrapper::Release()
{
	for (int i = 0; i < buffer.size(); ++i)
	{
		buffer[i]->Release();
	}
}

D3D12_GPU_VIRTUAL_ADDRESS KazBufferHelper::ID3D12ResourceWrapper::GetGpuAddress()
{
	return buffer[GetIndex()]->GetGPUVirtualAddress();
}

void* KazBufferHelper::ID3D12ResourceWrapper::GetMapAddres(int BB_INDEX)const
{
	void* dataMap = nullptr;
	if (BB_INDEX == -1)
	{
		buffer[GetIndex()]->Map(0, nullptr, (void**)&dataMap);
	}
	else
	{
		buffer[BB_INDEX]->Map(0, nullptr, (void**)&dataMap);
	}
	return dataMap;
}

void KazBufferHelper::ID3D12ResourceWrapper::CopyBuffer(const Microsoft::WRL::ComPtr<ID3D12Resource>& SRC_BUFFER) const
{
	for (int i = 0; i < buffer.size(); ++i)
	{
		DirectX12CmdList::Instance()->cmdList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(buffer[i].Get(),
				resourceState,
				D3D12_RESOURCE_STATE_COPY_DEST
			)
		);

		DirectX12CmdList::Instance()->cmdList->CopyResource(buffer[i].Get(), SRC_BUFFER.Get());

		DirectX12CmdList::Instance()->cmdList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(buffer[i].Get(),
				D3D12_RESOURCE_STATE_COPY_DEST,
				resourceState
			)
		);
	}
}

void KazBufferHelper::ID3D12ResourceWrapper::ChangeBarrier(D3D12_RESOURCE_STATES BEFORE_STATE, D3D12_RESOURCE_STATES AFTER_STATE)
{
	for (int i = 0; i < buffer.size(); ++i)
	{
		DirectX12CmdList::Instance()->cmdList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(buffer[i].Get(),
				BEFORE_STATE,
				AFTER_STATE
			)
		);
	}
	resourceState = AFTER_STATE;
}

void KazBufferHelper::ID3D12ResourceWrapper::ChangeBarrier(D3D12_RESOURCE_STATES AFTER_STATE)
{
	for (int i = 0; i < buffer.size(); ++i)
	{
		DirectX12CmdList::Instance()->cmdList->ResourceBarrier(
			1,
			&CD3DX12_RESOURCE_BARRIER::Transition(buffer[i].Get(),
				resourceState,
				AFTER_STATE
			)
		);
	}
	resourceState = AFTER_STATE;
}

const Microsoft::WRL::ComPtr<ID3D12Resource>& KazBufferHelper::ID3D12ResourceWrapper::GetBuffer(int INDEX) const
{
	if (INDEX == -1)
	{
		return buffer[GetIndex()];
	}
	else
	{
		return buffer[INDEX];
	}
}

void KazBufferHelper::ID3D12ResourceWrapper::operator=(const ID3D12ResourceWrapper& rhs)
{
	for (int i = 0; i < buffer.size(); ++i)
	{
		rhs.buffer[i].CopyTo(&buffer[i]);
	}
};

KazBufferHelper::BufferResourceData KazBufferHelper::SetGPUBufferData(BUFFER_SIZE BUFFER_SIZE, const std::string &BUFFER_NAME)
{
	D3D12_RESOURCE_DESC lDesc = CD3DX12_RESOURCE_DESC::Buffer(BUFFER_SIZE, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

	KazBufferHelper::BufferResourceData lData
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		lDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		BUFFER_NAME
	);
	lData.resourceDesc.Format;
	return lData;
}

KazBufferHelper::BufferResourceData KazBufferHelper::SetUploadBufferData(BUFFER_SIZE BUFFER_SIZE, const std::string& BUFFER_NAME)
{
	KazBufferHelper::BufferResourceData data
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		CD3DX12_RESOURCE_DESC::Buffer(BUFFER_SIZE),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		BUFFER_NAME
	);
	return data;
}

void KazBufferHelper::BufferData::GenerateCounterBuffer()
{
	counterWrapper = std::make_shared<ID3D12ResourceWrapper>(KazBufferHelper::SetGPUBufferData(sizeof(UINT), "CopyCounterBuffer-VRAM"));
	counterWrapper->ChangeBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

void KazBufferHelper::BufferData::CreateUAVView()
{
	RESOURCE_HANDLE handle = UavViewHandleMgr::Instance()->GetHandle();
	D3D12_UNORDERED_ACCESS_VIEW_DESC desc = KazBufferHelper::SetUnorderedAccessView(structureSize, elementNum);
	DescriptorHeapMgr::Instance()->CreateBufferView(handle, desc, bufferWrapper->GetBuffer().Get(), counterWrapper->GetBuffer().Get());
	bufferWrapper->CreateViewHandle(handle);
}

void KazBufferHelper::BufferData::operator=(const KazBufferHelper::BufferData& rhs)
{
	rangeType = rhs.rangeType;
	rootParamType = rhs.rootParamType;
	structureSize = rhs.structureSize;
	bufferWrapper = rhs.bufferWrapper;
	counterWrapper = rhs.counterWrapper;
	elementNum = rhs.elementNum;
};