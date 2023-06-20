#include "VertexBufferMgr.h"

RESOURCE_HANDLE VertexBufferMgr::GenerateBuffer(std::vector<VertexGenerateData> vertexData)
{
	vertexBufferArray.emplace_back();
	std::vector<KazRenderHelper::IASetVertexBuffersData> setVertDataArray;
	std::vector<D3D12_INDEX_BUFFER_VIEW> indexBufferViewArray;
	std::vector<KazRenderHelper::DrawIndexedInstancedData>drawCommandDataArray;


	RESOURCE_HANDLE outputHandle = handle.GetHandle();
	bool pushBackFlag = false;
	if (drawDataArray.size() <= outputHandle)
	{
		drawDataArray.emplace_back();
		pushBackFlag = true;
	}

	for (const auto &meshData : vertexData)
	{
		vertexBufferArray.back().emplace_back(std::make_unique<PolygonBuffer>());
		std::shared_ptr<KazBufferHelper::BufferData>vertexBuffer(vertexBufferArray.back().back()->GenerateVertexBuffer(meshData.verticesPos, meshData.structureSize, meshData.arraySize));
		std::shared_ptr<KazBufferHelper::BufferData>indexBuffer(vertexBufferArray.back().back()->GenerateIndexBuffer(meshData.indices));

		vertexBuffer->structureSize = meshData.structureSize;
		vertexBuffer->elementNum = static_cast<UINT>(meshData.arraySize);
		indexBuffer->structureSize = sizeof(USHORT);
		indexBuffer->elementNum = static_cast<UINT>(meshData.indices.size());

		//頂点情報
		setVertDataArray.emplace_back();
		setVertDataArray.back().numViews = 1;
		setVertDataArray.back().slot = 0;
		setVertDataArray.back().vertexBufferView = KazBufferHelper::SetVertexBufferView(vertexBuffer->bufferWrapper->GetGpuAddress(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(meshData.arraySize, meshData.structureSize), meshData.oneArraySize);

		//インデックス情報
		indexBufferViewArray.emplace_back(
			KazBufferHelper::SetIndexBufferView(
				indexBuffer->bufferWrapper->GetGpuAddress(),
				KazBufferHelper::GetBufferSize<BUFFER_SIZE>(meshData.indices.size(), sizeof(USHORT))
			)
		);

		//描画コマンド情報
		KazRenderHelper::DrawIndexedInstancedData result;
		result.indexCountPerInstance = static_cast<UINT>(meshData.indices.size());
		result.instanceCount = 1;
		result.startIndexLocation = 0;
		result.baseVertexLocation = 0;
		result.startInstanceLocation = 0;
		drawCommandDataArray.emplace_back(result);

		if (pushBackFlag)
		{
			drawDataArray.back().vertBuffer.emplace_back(vertexBuffer);
			drawDataArray.back().indexBuffer.emplace_back(indexBuffer);
		}
		else
		{
			drawDataArray[outputHandle].vertBuffer.emplace_back(vertexBuffer);
			drawDataArray[outputHandle].indexBuffer.emplace_back(indexBuffer);
		}
	}
	drawDataArray.back().index.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	drawDataArray.back().index.vertexBufferDrawData = setVertDataArray;
	drawDataArray.back().index.indexBufferView = indexBufferViewArray;
	drawDataArray.back().index.drawIndexInstancedData = drawCommandDataArray;

	return outputHandle;
}

RESOURCE_HANDLE VertexBufferMgr::GeneratePlaneBuffer()
{
	vertexBufferArray.back().emplace_back(std::make_unique<PolygonBuffer>());
	PolygonIndexData index = vertexBufferArray.back().back()->GeneratePlaneTexBuffer({ 1.0f,1.0f }, { 1,1 });


	std::shared_ptr<KazBufferHelper::BufferData>vertexBuffer(index.vertBuffer);
	std::shared_ptr<KazBufferHelper::BufferData>indexBuffer(index.indexBuffer);


	std::vector<KazRenderHelper::IASetVertexBuffersData> setVertDataArray;
	std::vector<D3D12_INDEX_BUFFER_VIEW> indexBufferViewArray;
	std::vector<KazRenderHelper::DrawIndexedInstancedData>drawCommandDataArray;

	//頂点情報
	setVertDataArray.emplace_back();
	setVertDataArray.back().numViews = 1;
	setVertDataArray.back().slot = 0;
	setVertDataArray.back().vertexBufferView = 
		KazBufferHelper::SetVertexBufferView(
			vertexBuffer->bufferWrapper->GetGpuAddress(),
			KazBufferHelper::GetBufferSize<BUFFER_SIZE>(4, sizeof(PolygonBuffer::VertUvData)),
			sizeof(PolygonBuffer::VertUvData)
		);

	//インデックス情報
	indexBufferViewArray.emplace_back(
		KazBufferHelper::SetIndexBufferView(
			indexBuffer->bufferWrapper->GetGpuAddress(),
			KazBufferHelper::GetBufferSize<BUFFER_SIZE>(6, sizeof(USHORT))
		)
	);

	//描画コマンド情報
	KazRenderHelper::DrawIndexedInstancedData result;
	result.indexCountPerInstance = static_cast<UINT>(6);
	result.instanceCount = 1;
	result.startIndexLocation = 0;
	result.baseVertexLocation = 0;
	result.startInstanceLocation = 0;
	drawCommandDataArray.emplace_back(result);

	drawDataArray.emplace_back();
	drawDataArray.back().vertBuffer.emplace_back(vertexBuffer);
	drawDataArray.back().indexBuffer.emplace_back(indexBuffer);
	drawDataArray.back().index.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	drawDataArray.back().index.vertexBufferDrawData = setVertDataArray;
	drawDataArray.back().index.indexBufferView = indexBufferViewArray;
	drawDataArray.back().index.drawIndexInstancedData = drawCommandDataArray;

	RESOURCE_HANDLE outputHandle = handle.GetHandle();
	return outputHandle;
}

void VertexBufferMgr::ReleaseBuffer(RESOURCE_HANDLE HANDLE)
{
	drawDataArray[HANDLE].index.Finalize();
	for (auto &vertBuffer : drawDataArray[HANDLE].vertBuffer)
	{
		vertBuffer.reset();
	}
	for (auto &indexBuffer : drawDataArray[HANDLE].indexBuffer)
	{
		indexBuffer.reset();
	}
}

PolygonMultiMeshedIndexData VertexBufferMgr::GetBuffer(RESOURCE_HANDLE HANDLE)
{
	return drawDataArray[HANDLE];
}