#include "VertexBufferMgr.h"
#include"DescriptorHeapMgr.h"

RESOURCE_HANDLE VertexBufferMgr::GenerateBuffer(const std::vector<VertexGenerateData>& vertexData)
{
	std::vector<KazRenderHelper::IASetVertexBuffersData> setVertDataArray;
	std::vector<D3D12_INDEX_BUFFER_VIEW> indexBufferViewArray;
	std::vector<KazRenderHelper::DrawIndexedInstancedData>drawCommandDataArray;


	RESOURCE_HANDLE outputHandle = m_handle.GetHandle();
	bool pushBackFlag = false;
	if (drawDataArray.size() <= outputHandle)
	{
		drawDataArray.emplace_back();
		m_polygonBufferArray.emplace_back();
		pushBackFlag = true;
	}

	for (const auto& meshData : vertexData)
	{
		m_polygonBufferArray[outputHandle].emplace_back(
			PolygonGenerateData(meshData.verticesPos, meshData.structureSize, meshData.arraySize),
			PolygonGenerateData((void*)meshData.indices.data(), sizeof(UINT), meshData.indices.size())
		);

		std::shared_ptr<KazBufferHelper::BufferData>vertexBuffer(m_polygonBufferArray[outputHandle].back().m_gpuBuffer.m_vertexBuffer);
		std::shared_ptr<KazBufferHelper::BufferData>indexBuffer(m_polygonBufferArray[outputHandle].back().m_gpuBuffer.m_indexBuffer);

		vertexBuffer->structureSize = meshData.structureSize;
		vertexBuffer->elementNum = static_cast<UINT>(meshData.arraySize);
		indexBuffer->structureSize = sizeof(UINT);
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
				KazBufferHelper::GetBufferSize<BUFFER_SIZE>(meshData.indices.size(), sizeof(UINT))
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


		drawDataArray[outputHandle].vertBuffer.emplace_back(vertexBuffer);
		drawDataArray[outputHandle].indexBuffer.emplace_back(indexBuffer);


		RESOURCE_HANDLE handle = DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_IAPOLYGONE).startSize + sDescHandle;
		D3D12_SHADER_RESOURCE_VIEW_DESC view;
		view.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		view.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		view.Format = DXGI_FORMAT_UNKNOWN;
		view.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		view.Buffer.NumElements = static_cast<UINT>(meshData.arraySize);
		view.Buffer.FirstElement = 0;
		view.Buffer.StructureByteStride = meshData.structureSize;
		DescriptorHeapMgr::Instance()->CreateBufferView(handle, view, vertexBuffer->bufferWrapper->GetBuffer().Get());
		drawDataArray[outputHandle].vertBuffer.back()->bufferWrapper->CreateViewHandle(handle);
		++sDescHandle;


		handle = DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_IAPOLYGONE).startSize + sDescHandle;
		view.Buffer.NumElements = static_cast<UINT>(meshData.indices.size());
		view.Buffer.StructureByteStride = sizeof(UINT);
		DescriptorHeapMgr::Instance()->CreateBufferView(handle, view, indexBuffer->bufferWrapper->GetBuffer().Get());
		drawDataArray[outputHandle].indexBuffer.back()->bufferWrapper->CreateViewHandle(handle);
		++sDescHandle;
	}
	drawDataArray[outputHandle].index.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	drawDataArray[outputHandle].index.vertexBufferDrawData = setVertDataArray;
	drawDataArray[outputHandle].index.indexBufferView = indexBufferViewArray;
	drawDataArray[outputHandle].index.drawIndexInstancedData = drawCommandDataArray;

	return outputHandle;
}

RESOURCE_HANDLE VertexBufferMgr::GenerateBufferWithoutIndex(const std::vector<VertexGenerateData>& vertexData)
{
	std::vector<KazRenderHelper::IASetVertexBuffersData> setVertDataArray;
	std::vector<KazRenderHelper::DrawIndexedInstancedData> drawCommandDataArray;


	RESOURCE_HANDLE outputHandle = m_handle.GetHandle();
	bool pushBackFlag = false;
	if (drawDataArray.size() <= outputHandle)
	{
		drawDataArray.emplace_back();
		m_polygonBufferArray.emplace_back();
		pushBackFlag = true;
	}

	for (const auto& meshData : vertexData)
	{
		m_polygonBufferArray[outputHandle].emplace_back(
			PolygonGenerateData(meshData.verticesPos, meshData.structureSize, meshData.arraySize)
		);

		std::shared_ptr<KazBufferHelper::BufferData>vertexBuffer(m_polygonBufferArray[outputHandle].back().m_cpuBuffer.m_vertexBuffer);

		vertexBuffer->structureSize = meshData.structureSize;
		vertexBuffer->elementNum = static_cast<UINT>(meshData.arraySize);

		//頂点情報
		setVertDataArray.emplace_back();
		setVertDataArray.back().numViews = 1;
		setVertDataArray.back().slot = 0;
		setVertDataArray.back().vertexBufferView = KazBufferHelper::SetVertexBufferView(vertexBuffer->bufferWrapper->GetGpuAddress(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(meshData.arraySize, meshData.structureSize), meshData.oneArraySize);


		//描画コマンド情報
		KazRenderHelper::DrawIndexedInstancedData result;
		result.indexCountPerInstance = static_cast<UINT>(meshData.indices.size());
		result.instanceCount = 1;
		result.startIndexLocation = 0;
		result.baseVertexLocation = 0;
		result.startInstanceLocation = 0;
		drawCommandDataArray.emplace_back(result);


		drawDataArray[outputHandle].vertBuffer.emplace_back(vertexBuffer);


		RESOURCE_HANDLE handle = DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_IAPOLYGONE).startSize + sDescHandle;
		D3D12_SHADER_RESOURCE_VIEW_DESC view;
		view.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		view.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		view.Format = DXGI_FORMAT_UNKNOWN;
		view.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		view.Buffer.NumElements = static_cast<UINT>(meshData.arraySize);
		view.Buffer.FirstElement = 0;
		view.Buffer.StructureByteStride = meshData.structureSize;
		DescriptorHeapMgr::Instance()->CreateBufferView(handle, view, vertexBuffer->bufferWrapper->GetBuffer().Get());
		drawDataArray[outputHandle].vertBuffer.back()->bufferWrapper->CreateViewHandle(handle);
		++sDescHandle;
	}
	drawDataArray[outputHandle].index.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	drawDataArray[outputHandle].index.vertexBufferDrawData = setVertDataArray;
	drawDataArray[outputHandle].index.drawIndexInstancedData = drawCommandDataArray;

	return outputHandle;
}

RESOURCE_HANDLE VertexBufferMgr::GeneratePlaneBuffer()
{
	vertexBufferArray.emplace_back();
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
			KazBufferHelper::GetBufferSize<BUFFER_SIZE>(6, sizeof(UINT))
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

	RESOURCE_HANDLE outputHandle = m_handle.GetHandle();
	return outputHandle;
}

RESOURCE_HANDLE VertexBufferMgr::GenerateBoxBuffer()
{
	vertexBufferArray.emplace_back();
	vertexBufferArray.back().emplace_back(std::make_unique<PolygonBuffer>());
	PolygonIndexData index = vertexBufferArray.back().back()->GenerateBoxBuffer(1.0f);


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
			KazBufferHelper::GetBufferSize<BUFFER_SIZE>(6, sizeof(UINT))
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

	RESOURCE_HANDLE outputHandle = m_handle.GetHandle();
	return outputHandle;
}

void VertexBufferMgr::ReleaseBuffer(RESOURCE_HANDLE HANDLE)
{
	drawDataArray[HANDLE].index.Finalize();
	for (auto& vertBuffer : drawDataArray[HANDLE].vertBuffer)
	{
		vertBuffer.reset();
	}
	for (auto& indexBuffer : drawDataArray[HANDLE].indexBuffer)
	{
		indexBuffer.reset();
	}
}

PolygonMultiMeshedIndexData VertexBufferMgr::GetBuffer(RESOURCE_HANDLE HANDLE)
{
	return drawDataArray[HANDLE];
}