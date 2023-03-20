#pragma once
#include"../KazLibrary/Buffer/CreateGpuBuffer.h"
#include"../KazLibrary/Helper/ResouceBufferHelper.h"

/// <summary>
/// ÉÇÉfÉãÇÃí∏ì_èÓïÒÇVRAMè„Ç…ï€ë∂Ç∑ÇÈ
/// </summary>
class CreateMeshBuffer
{
public:
	struct BufferHandle
	{
		RESOURCE_HANDLE bufferHandle;
		RESOURCE_HANDLE descriptorViewHandle;

		BufferHandle():bufferHandle(-1), descriptorViewHandle(-1)
		{
		};
	};

	enum MeshBufferView
	{
		DATA_VERT,
		DATA_UV,
		DATA_NORMAL,
		DATA_MAX
	};

	CreateMeshBuffer(RESOURCE_HANDLE HANDLE);
	CreateMeshBuffer(std::vector<DirectX::XMFLOAT3> VERT, std::vector<DirectX::XMFLOAT2> UV);
	const ResouceBufferHelper::BufferData &GetBufferData(MeshBufferView ENUM_VIEW);
	
	bool IsDataIn(MeshBufferView ENUM_VIEW)
	{
		RESOURCE_HANDLE lHandle = static_cast<RESOURCE_HANDLE>(ENUM_VIEW);
		if (bufferHandleDataArray[lHandle].bufferHandle != -1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
private:
	std::array<BufferHandle, DATA_MAX>bufferHandleDataArray;

	ResouceBufferHelper computeHelper;
};

