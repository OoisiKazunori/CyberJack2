#pragma once
#include"../KazLibrary/Buffer/CreateGpuBuffer.h"
#include"../KazLibrary/Helper/ResouceBufferHelper.h"
#include"../KazLibrary/Helper/Compute.h"

/// <summary>
/// モデルの頂点情報をVRAM上に保存する
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

	CreateMeshBuffer(std::vector<DirectX::XMFLOAT3> VERT, std::vector<DirectX::XMFLOAT2> UV);
	CreateMeshBuffer(std::vector<KazMath::Vec3<float>> VERT, std::vector<KazMath::Vec2<float>> UV);
	const KazBufferHelper::BufferData &GetBufferData(MeshBufferView ENUM_VIEW);
	
	bool IsDataIn(MeshBufferView ENUM_VIEW)
	{
		RESOURCE_HANDLE lHandle = static_cast<RESOURCE_HANDLE>(ENUM_VIEW);
		if (bufferHandleDataArray[lHandle].bufferWrapper)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
private:
	std::array<KazBufferHelper::BufferData, DATA_MAX>bufferHandleDataArray;

	ComputeShader computeHelper;
};

