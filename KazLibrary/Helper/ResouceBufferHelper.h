#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"

struct DispatchCallData
{
	UINT x, y, z;
};

class ResouceBufferHelper
{
public:

	struct BufferData
	{
		KazBufferHelper::ID3D12ResourceWrapper bufferWrapper;
		KazBufferHelper::ID3D12ResourceWrapper counterWrapper;
		GraphicsRangeType rangeType;
		GraphicsRootParamType rootParamType;
		UINT bufferSize;
		UINT elementNum;

		BufferData(const KazBufferHelper::BufferResourceData &BUFFER_DATA) :rangeType(GRAPHICS_RANGE_TYPE_NONE), rootParamType(GRAPHICS_PRAMTYPE_NONE), bufferSize(0), elementNum(0)
		{
			bufferWrapper.CreateBuffer(BUFFER_DATA);
		}
		BufferData() :rangeType(GRAPHICS_RANGE_TYPE_NONE), rootParamType(GRAPHICS_PRAMTYPE_NONE), bufferSize(0), elementNum(0)
		{
		}

		void CreateViewHandle(std::vector<RESOURCE_HANDLE>HANDLE_ARRAY)
		{
			viewHandle = HANDLE_ARRAY;
		}
		void CreateViewHandle(RESOURCE_HANDLE HANDLE)
		{
			viewHandle.emplace_back(HANDLE);
		}
		const RESOURCE_HANDLE &GetViewHandle()const
		{
			return viewHandle[GetIndex()];
		}

		void operator=(const BufferData &rhs)
		{
			rangeType = rhs.rangeType;
			rootParamType = rhs.rootParamType;
			bufferSize = rhs.bufferSize;
			bufferWrapper = rhs.bufferWrapper;
			counterWrapper = rhs.counterWrapper;
			viewHandle = rhs.viewHandle;
			elementNum = rhs.elementNum;
		};

	private:
		std::vector<RESOURCE_HANDLE> viewHandle;
	};

	ResouceBufferHelper();

	RESOURCE_HANDLE CreateBuffer(UINT STRUCTURE_BYTE_STRIDE, GraphicsRangeType RANGE, GraphicsRootParamType ROOTPARAM, UINT ELEMENT_NUM, bool GENERATE_COUNTER_BUFFER_FLAG = false);
	RESOURCE_HANDLE CreateBuffer(const KazBufferHelper::BufferResourceData &BUFFER_OPTION_DATA, GraphicsRangeType RANGE, GraphicsRootParamType ROOTPARAM, UINT STRUCTURE_BYTE_STRIDE, UINT ELEMENT_NUM, bool GENERATE_COUNTER_BUFFER_FLAG = false);

	ResouceBufferHelper::BufferData CreateAndGetBuffer(UINT STRUCTURE_BYTE_STRIDE, GraphicsRangeType RANGE, GraphicsRootParamType ROOTPARAM, UINT ELEMENT_NUM, bool GENERATE_COUNTER_BUFFER_FLAG = false);
	ResouceBufferHelper::BufferData CreateAndGetBuffer(const KazBufferHelper::BufferResourceData &BUFFER_OPTION_DATA, GraphicsRangeType RANGE, GraphicsRootParamType ROOTPARAM, UINT STRUCTURE_BYTE_STRIDE, UINT ELEMENT_NUM, bool GENERATE_COUNTER_BUFFER_FLAG = false);

	RESOURCE_HANDLE SetBuffer(const ResouceBufferHelper::BufferData &DATA, GraphicsRootParamType ROOTPARAM);

	void TransData(RESOURCE_HANDLE HANDLE, void *TRANS_DATA, UINT TRANSMISSION_DATA_SIZE);

	void StackToCommandListAndCallDispatch(ComputePipeLineNames NAME, const DispatchCallData &DISPATCH_DATA, UINT ADJ_NUM = 0);
	void StackToCommandList(PipeLineNames NAME);

	void DeleteAllData();


	void InitCounterBuffer(const Microsoft::WRL::ComPtr<ID3D12Resource> &INIT_DATA);
	const ResouceBufferHelper::BufferData &GetBufferData(RESOURCE_HANDLE HANDLE)const;
	void SetRootParam(RESOURCE_HANDLE HANDLE, GraphicsRootParamType ROOT_PARAM);

private:
	std::vector<ResouceBufferHelper::BufferData>bufferArrayData;
	HandleMaker handle;

	static const int SWAPCHAIN_NUM;


	KazBufferHelper::BufferResourceData counterBufferData;
	static const UINT GetIndex()
	{
		if (SWAPCHAIN_NUM <= RenderTargetStatus::Instance()->bbIndex)
		{
			return 0;
		}
		else
		{
			return RenderTargetStatus::Instance()->bbIndex;
		}
	}
};