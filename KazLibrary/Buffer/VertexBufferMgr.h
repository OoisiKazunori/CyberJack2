#pragma once
#include"Polygon.h"
#include"../KazLibrary/Helper/ISinglton.h"

/// <summary>
/// 頂点情報
/// </summary>
struct VertexGenerateData
{
	void *verticesPos;
	int structureSize;
	size_t arraySize;
	int oneArraySize;
	std::vector<USHORT>indices;

	VertexGenerateData(void *VERT_PTR, int STRUCTURE_SIZE, size_t ARRAY_SIZE, int ONE_ARRAY_SIZE, std::vector<USHORT>INDICES) :
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
	RESOURCE_HANDLE GenerateBuffer(std::vector<VertexGenerateData> vertexData);
	RESOURCE_HANDLE GeneratePlaneBuffer();

	void ReleaseBuffer(RESOURCE_HANDLE HANDLE);
	PolygonMultiMeshedIndexData GetBuffer(RESOURCE_HANDLE HANDLE);

private:
	HandleMaker handle;
	std::vector<std::vector<std::unique_ptr<PolygonBuffer>>>vertexBufferArray;
	std::vector<PolygonMultiMeshedIndexData>drawDataArray;
};
