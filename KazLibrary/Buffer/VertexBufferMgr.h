#pragma once
#include"Polygon.h"
#include"../KazLibrary/Helper/ISinglton.h"

struct IAPolygonData
{
	std::shared_ptr<KazBufferHelper::BufferData> m_vertexBuffer;
	std::shared_ptr<KazBufferHelper::BufferData> m_indexBuffer;

	IAPolygonData(const KazBufferHelper::BufferData& arg_vertexBufferGennerateData, const KazBufferHelper::BufferData& arg_indexBufferGennerateData) :
		m_vertexBuffer(std::make_shared<KazBufferHelper::BufferData>(arg_vertexBufferGennerateData)), m_indexBuffer(std::make_shared<KazBufferHelper::BufferData>(arg_indexBufferGennerateData))
	{}

	IAPolygonData(const KazBufferHelper::BufferData& arg_vertexBufferGennerateData) :
		m_vertexBuffer(std::make_shared<KazBufferHelper::BufferData>(arg_vertexBufferGennerateData))
	{}
};

struct PolygonGenerateData
{
	void* m_ptr;
	int m_structureSize;
	size_t m_arraySize;
};

struct IAPolygonBufferData
{
	IAPolygonData m_cpuBuffer, m_gpuBuffer;

	IAPolygonBufferData(const PolygonGenerateData& arg_vertex, const PolygonGenerateData& arg_index) :
		m_cpuBuffer(
			KazBufferHelper::SetVertexBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(arg_vertex.m_arraySize, arg_vertex.m_structureSize)),
			KazBufferHelper::SetIndexBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(arg_index.m_arraySize, arg_index.m_structureSize))
			),
		m_gpuBuffer(
			KazBufferHelper::SetGPUBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(arg_vertex.m_arraySize, arg_vertex.m_structureSize)),
			KazBufferHelper::SetGPUBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(arg_index.m_arraySize, arg_index.m_structureSize))
		)
	{
		m_cpuBuffer.m_vertexBuffer->bufferWrapper->TransData(arg_vertex.m_ptr, KazBufferHelper::GetBufferSize<BUFFER_SIZE>(arg_vertex.m_arraySize, arg_vertex.m_structureSize));
		m_cpuBuffer.m_indexBuffer->bufferWrapper->TransData(arg_index.m_ptr, KazBufferHelper::GetBufferSize<BUFFER_SIZE>(arg_index.m_arraySize, arg_index.m_structureSize));

		m_gpuBuffer.m_vertexBuffer->bufferWrapper->CopyBuffer(m_cpuBuffer.m_vertexBuffer->bufferWrapper->GetBuffer(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
		m_gpuBuffer.m_indexBuffer->bufferWrapper->CopyBuffer(m_cpuBuffer.m_indexBuffer->bufferWrapper->GetBuffer(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	};

	IAPolygonBufferData(const PolygonGenerateData& arg_vertex) :
		m_cpuBuffer(
			KazBufferHelper::SetVertexBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(arg_vertex.m_arraySize, arg_vertex.m_structureSize))
		),
		m_gpuBuffer(
			KazBufferHelper::SetGPUBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(arg_vertex.m_arraySize, arg_vertex.m_structureSize))
		)
	{
		m_cpuBuffer.m_vertexBuffer->bufferWrapper->TransData(arg_vertex.m_ptr, KazBufferHelper::GetBufferSize<BUFFER_SIZE>(arg_vertex.m_arraySize, arg_vertex.m_structureSize));
		m_gpuBuffer.m_vertexBuffer->bufferWrapper->CopyBuffer(m_cpuBuffer.m_vertexBuffer->bufferWrapper->GetBuffer(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
	};
};

/// <summary>
/// 頂点情報
/// </summary>
struct VertexGenerateData
{
	void* verticesPos;
	int structureSize;
	size_t arraySize;
	int oneArraySize;
	std::vector<UINT>indices;

	VertexGenerateData(void* VERT_PTR, int STRUCTURE_SIZE, size_t ARRAY_SIZE, int ONE_ARRAY_SIZE, std::vector<UINT>INDICES) :
		verticesPos(VERT_PTR), structureSize(STRUCTURE_SIZE), arraySize(ARRAY_SIZE), oneArraySize(ONE_ARRAY_SIZE), indices(INDICES)
	{};
};

/// <summary>
/// ラスタライザ描画用の頂点バッファ管理
/// 基本使い回し
/// </summary>
class VertexBufferMgr :public ISingleton<VertexBufferMgr>
{
public:
	RESOURCE_HANDLE GenerateBuffer(const std::vector<VertexGenerateData>& vertexData);
	RESOURCE_HANDLE GenerateBufferWithoutIndex(const std::vector<VertexGenerateData>& vertexData);
	RESOURCE_HANDLE GeneratePlaneBuffer();
	RESOURCE_HANDLE GenerateBoxBuffer();

	void ReleaseBuffer(RESOURCE_HANDLE HANDLE);
	PolygonMultiMeshedIndexData GetBuffer(RESOURCE_HANDLE HANDLE);

private:
	HandleMaker m_handle;
	std::vector<std::vector<std::unique_ptr<PolygonBuffer>>>vertexBufferArray;
	std::vector<PolygonMultiMeshedIndexData>drawDataArray;

	std::vector<std::vector<IAPolygonBufferData>>m_polygonBufferArray;
	int sDescHandle = 0;
};
