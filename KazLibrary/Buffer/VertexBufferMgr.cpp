#include "VertexBufferMgr.h"
#include"DescriptorHeapMgr.h"

RESOURCE_HANDLE VertexBufferMgr::GenerateBuffer(const std::vector<VertexAndIndexGenerateData>& vertexData)
{
	std::vector<KazRenderHelper::IASetVertexBuffersData> setVertDataArray;
	std::vector<D3D12_INDEX_BUFFER_VIEW> indexBufferViewArray;
	std::vector<KazRenderHelper::DrawIndexedInstancedData>drawCommandDataArray;


	RESOURCE_HANDLE outputHandle = m_handle.GetHandle();
	bool pushBackFlag = false;
	if (m_drawIndexDataArray.size() <= outputHandle)
	{
		m_drawIndexDataArray.emplace_back();
		m_polygonIndexBufferArray.emplace_back();
		pushBackFlag = true;
	}

	for (const auto& meshData : vertexData)
	{
		m_polygonIndexBufferArray[outputHandle].emplace_back();
		m_polygonIndexBufferArray[outputHandle].back().emplace_back(PolygonGenerateData(meshData.verticesPos, meshData.structureSize, meshData.arraySize));
		m_polygonIndexBufferArray[outputHandle].back().emplace_back(PolygonGenerateData((void*)meshData.indices.data(), sizeof(UINT), meshData.indices.size()));

		std::shared_ptr<KazBufferHelper::BufferData>vertexBuffer(m_polygonIndexBufferArray[outputHandle].back()[0].m_gpuBuffer.m_buffer);
		std::shared_ptr<KazBufferHelper::BufferData>indexBuffer(m_polygonIndexBufferArray[outputHandle].back()[1].m_gpuBuffer.m_buffer);

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


		m_drawIndexDataArray[outputHandle].vertBuffer.emplace_back(vertexBuffer);
		m_drawIndexDataArray[outputHandle].indexBuffer.emplace_back(indexBuffer);


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
		m_drawIndexDataArray[outputHandle].vertBuffer.back()->bufferWrapper->CreateViewHandle(handle);
		++sDescHandle;


		handle = DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_IAPOLYGONE).startSize + sDescHandle;
		view.Buffer.NumElements = static_cast<UINT>(meshData.indices.size());
		view.Buffer.StructureByteStride = sizeof(UINT);
		DescriptorHeapMgr::Instance()->CreateBufferView(handle, view, indexBuffer->bufferWrapper->GetBuffer().Get());
		m_drawIndexDataArray[outputHandle].indexBuffer.back()->bufferWrapper->CreateViewHandle(handle);
		++sDescHandle;
	}
	m_drawIndexDataArray[outputHandle].index.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_drawIndexDataArray[outputHandle].index.vertexBufferDrawData = setVertDataArray;
	m_drawIndexDataArray[outputHandle].index.indexBufferView = indexBufferViewArray;
	m_drawIndexDataArray[outputHandle].index.drawIndexInstancedData = drawCommandDataArray;

	return outputHandle;
}

RESOURCE_HANDLE VertexBufferMgr::GenerateBuffer(const VertexGenerateData& arg_vertexData, bool arg_generateInVRAMFlag)
{
	RESOURCE_HANDLE outputHandle = m_vertexHandle.GetHandle();
	bool pushBackFlag = false;
	if (m_drawDataArray.size() <= outputHandle)
	{
		m_drawDataArray.emplace_back();
		m_polygonBufferArray.emplace_back(PolygonGenerateData(arg_vertexData.verticesPos, arg_vertexData.structureSize, arg_vertexData.arraySize));
		pushBackFlag = true;
	}
	else
	{
		m_polygonBufferArray[outputHandle] = PolygonGenerateData(arg_vertexData.verticesPos, arg_vertexData.structureSize, arg_vertexData.arraySize);
	}

	std::shared_ptr<KazBufferHelper::BufferData>vertexBuffer;
	if (arg_generateInVRAMFlag)
	{
		vertexBuffer = m_polygonBufferArray[outputHandle].m_gpuBuffer.m_buffer;
	}
	else
	{
		vertexBuffer = m_polygonBufferArray[outputHandle].m_cpuBuffer.m_buffer;
	}
	vertexBuffer->structureSize = arg_vertexData.structureSize;
	vertexBuffer->elementNum = static_cast<UINT>(arg_vertexData.arraySize);
	m_drawDataArray[outputHandle].vertBuffer = vertexBuffer;

	//頂点情報
	KazRenderHelper::IASetVertexBuffersData setVertDataArray;
	setVertDataArray.numViews = 1;
	setVertDataArray.slot = 0;
	setVertDataArray.vertexBufferView = KazBufferHelper::SetVertexBufferView(vertexBuffer->bufferWrapper->GetGpuAddress(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(arg_vertexData.arraySize, arg_vertexData.structureSize), arg_vertexData.oneArraySize);


	//頂点ビュー
	RESOURCE_HANDLE handle = DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_IAPOLYGONE).startSize + sDescHandle;
	D3D12_SHADER_RESOURCE_VIEW_DESC view;
	view.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	view.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	view.Format = DXGI_FORMAT_UNKNOWN;
	view.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	view.Buffer.NumElements = static_cast<UINT>(arg_vertexData.arraySize);
	view.Buffer.FirstElement = 0;
	view.Buffer.StructureByteStride = arg_vertexData.structureSize;
	DescriptorHeapMgr::Instance()->CreateBufferView(handle, view, vertexBuffer->bufferWrapper->GetBuffer().Get());
	m_drawDataArray[outputHandle].vertBuffer->bufferWrapper->CreateViewHandle(handle);
	++sDescHandle;


	m_drawDataArray[outputHandle].instanceData.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_drawDataArray[outputHandle].instanceData.vertexBufferDrawData = setVertDataArray;
	//描画コマンド情報
	KazRenderHelper::DrawInstancedData result;
	result.vertexCountPerInstance = static_cast<UINT>(arg_vertexData.arraySize);
	result.instanceCount = 1;
	result.startInstanceLocation = 0;
	result.startVertexLocation = 0;
	m_drawDataArray[outputHandle].instanceData.drawInstanceData = result;

	return outputHandle;
}

RESOURCE_HANDLE VertexBufferMgr::GeneratePlaneBuffer()
{
	m_vertexBufferArray.emplace_back();
	m_vertexBufferArray.back().emplace_back(std::make_unique<PolygonBuffer>());
	PolygonIndexData index = m_vertexBufferArray.back().back()->GeneratePlaneTexBuffer({ 1.0f,1.0f }, { 1,1 });


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

	m_drawIndexDataArray.emplace_back();
	m_drawIndexDataArray.back().vertBuffer.emplace_back(vertexBuffer);
	m_drawIndexDataArray.back().indexBuffer.emplace_back(indexBuffer);
	m_drawIndexDataArray.back().index.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_drawIndexDataArray.back().index.vertexBufferDrawData = setVertDataArray;
	m_drawIndexDataArray.back().index.indexBufferView = indexBufferViewArray;
	m_drawIndexDataArray.back().index.drawIndexInstancedData = drawCommandDataArray;

	RESOURCE_HANDLE outputHandle = m_handle.GetHandle();
	return outputHandle;
}

RESOURCE_HANDLE VertexBufferMgr::GenerateBoxBuffer()
{
	m_vertexBufferArray.emplace_back();
	m_vertexBufferArray.back().emplace_back(std::make_unique<PolygonBuffer>());
	PolygonIndexData index = m_vertexBufferArray.back().back()->GenerateBoxBuffer(1.0f);


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

	m_drawIndexDataArray.emplace_back();
	m_drawIndexDataArray.back().vertBuffer.emplace_back(vertexBuffer);
	m_drawIndexDataArray.back().indexBuffer.emplace_back(indexBuffer);
	m_drawIndexDataArray.back().index.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_drawIndexDataArray.back().index.vertexBufferDrawData = setVertDataArray;
	m_drawIndexDataArray.back().index.indexBufferView = indexBufferViewArray;
	m_drawIndexDataArray.back().index.drawIndexInstancedData = drawCommandDataArray;

	RESOURCE_HANDLE outputHandle = m_handle.GetHandle();
	return outputHandle;
}

void VertexBufferMgr::ReleaseVeretexIndexBuffer(RESOURCE_HANDLE HANDLE)
{
	m_handle.DeleteHandle(HANDLE);
	m_drawIndexDataArray[HANDLE].index.Finalize();
	for (auto& vertBuffer : m_drawIndexDataArray[HANDLE].vertBuffer)
	{
		vertBuffer.reset();
	}
	for (auto& indexBuffer : m_drawIndexDataArray[HANDLE].indexBuffer)
	{
		indexBuffer.reset();
	}
}

void VertexBufferMgr::ReleaseVeretexBuffer(RESOURCE_HANDLE HANDLE)
{
	m_vertexHandle.DeleteHandle(HANDLE);
	m_drawDataArray[HANDLE].vertBuffer.reset();
}

PolygonMultiMeshedIndexData VertexBufferMgr::GetVertexIndexBuffer(RESOURCE_HANDLE HANDLE)
{
	return m_drawIndexDataArray[HANDLE];
}

PolygonInstanceData VertexBufferMgr::GetVertexBuffer(RESOURCE_HANDLE HANDLE)
{
	return m_drawDataArray[HANDLE];
}
