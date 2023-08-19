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

	CreateMeshBuffer(std::vector<DirectX::XMFLOAT3> VERT, std::vector<DirectX::XMFLOAT2> UV, std::vector<UINT>INDICES = std::vector<UINT>());
	CreateMeshBuffer(std::vector<KazMath::Vec3<float>> VERT, std::vector<KazMath::Vec2<float>> UV, std::vector<UINT>INDICES = std::vector<UINT>());
	const KazBufferHelper::BufferData &GetBufferData(MeshBufferView ENUM_VIEW);
	
	bool IsDataIn(MeshBufferView ENUM_VIEW)
	{
		RESOURCE_HANDLE lHandle = static_cast<RESOURCE_HANDLE>(ENUM_VIEW);
		if (m_uploadBufferHandleDataArray[lHandle].bufferWrapper)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
private:
	std::array<KazBufferHelper::BufferData, DATA_MAX>m_uploadBufferHandleDataArray;
	std::array<KazBufferHelper::BufferData, DATA_MAX>m_VRAMBufferHandleDataArray;

	ComputeShader m_computeHelper;

	void GenerateBuffer(MeshBufferView TYPE, GraphicsRootParamType ROOT_TYPE, size_t SIZE, unsigned long long STRUCTER_SIZE, void* ADDRESS, std::string BUFFER_NAME);
	void UploadToVRAM();

	//重複した頂点情報に変換する
	template<typename T>
	std::vector<T> Convert(const std::vector<T>& arg_array, const std::vector<UINT>& arg_indicesArray)
	{
		std::vector<T>array;
		for (auto& obj : arg_indicesArray)
		{
			array.emplace_back(arg_array[obj]);
		}
		return array;
	}
};

