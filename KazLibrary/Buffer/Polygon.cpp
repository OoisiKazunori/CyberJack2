#include"Polygon.h"

std::shared_ptr<KazBufferHelper::BufferData> PolygonBuffer::GenerateVertexBuffer(void *verticesPos, int structureSize, size_t arraySize)
{
	BUFFER_SIZE lSize = static_cast<BUFFER_SIZE>(arraySize * structureSize);
	//RAMのバッファに一度転送---------------------------------------
	cpuVertBuffer = std::make_shared<KazBufferHelper::BufferData>(KazBufferHelper::SetVertexBufferData(lSize));
	cpuVertBuffer->bufferWrapper.TransData(verticesPos, lSize);
	//RAMのバッファに一度転送---------------------------------------

	//VRAMのバッファにコピー---------------------------------------
	vertBuffer = std::make_shared<KazBufferHelper::BufferData>(KazBufferHelper::SetGPUBufferData(lSize));
	vertBuffer->bufferWrapper.CopyBuffer(cpuVertBuffer->bufferWrapper.GetBuffer(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST);
	//VRAMのバッファにコピー---------------------------------------

	return cpuVertBuffer;
}

std::shared_ptr<KazBufferHelper::BufferData> PolygonBuffer::GenerateIndexBuffer(std::vector<USHORT> indices)
{
	BUFFER_SIZE lSize = static_cast<BUFFER_SIZE>(indices.size() * sizeof(USHORT));
	//RAMのバッファに一度転送---------------------------------------
	cpuIndexBuffer = std::make_shared<KazBufferHelper::BufferData>(KazBufferHelper::SetIndexBufferData(lSize));
	cpuIndexBuffer->bufferWrapper.TransData(indices.data(), lSize);
	//RAMのバッファに一度転送---------------------------------------

	//VRAMのバッファにコピー---------------------------------------
	//indexBuffer = std::make_shared<KazBufferHelper::BufferData>(KazBufferHelper::SetGPUBufferData(lSize));
	//indexBuffer->bufferWrapper.CopyBuffer(cpuIndexBuffer->bufferWrapper.GetBuffer(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST);
	//VRAMのバッファにコピー---------------------------------------

	return cpuIndexBuffer;
}

PolygonIndexData PolygonBuffer::GenerateBoxBuffer(float scale)
{
	const float lSize = scale;

	std::vector<DirectX::XMFLOAT3> lVertices =
	{
	//前
	{-lSize, -lSize, -lSize},	//左下
	{-lSize,  lSize, -lSize},		//左上
	{ lSize, -lSize, -lSize},		//右下
	{ lSize,  lSize, -lSize},		//右上

	//後
	{-lSize,-lSize,  lSize}, 	//左下
	{-lSize, lSize,  lSize}, 	//左上
	{ lSize,-lSize,  lSize},	//右下
	{ lSize, lSize,  lSize},	//右上

	//左
	{-lSize,-lSize, -lSize},	//左下
	{-lSize,-lSize,  lSize}, 	//左上
	{-lSize, lSize, -lSize}, 	//右下
	{-lSize, lSize,  lSize}, 	//右上

	//右
	{lSize,-lSize, -lSize},	//左下
	{lSize,-lSize,  lSize},	//左上
	{lSize, lSize, -lSize},	//右下
	{lSize, lSize,  lSize},	//右上

	//下
	{ lSize, -lSize, lSize}, 	//左下
	{ lSize, -lSize,-lSize}, 	//左上
	{-lSize, -lSize, lSize}, 	//右下
	{-lSize, -lSize,-lSize}, //右上

	//上
	{ lSize, lSize, lSize},		//左下
	{ lSize, lSize,-lSize},		//左上
	{-lSize, lSize, lSize},		//右下
	{-lSize, lSize,-lSize}	//右上
	};

	std::vector<USHORT> lIndices =
	{
		//前
		0,1,2,		//三角形1つ目
		2,1,3,		//三角形2つ目
		//後
		4,6,5,		//三角形3つ目
		6,7,5,		//三角形4つ目
		//左
		8,9,10,		//三角形1つ目
		10,9,11,	//三角形2つ目
		//右
		12,14,13,	//三角形1つ目
		13,14,15,	//三角形2つ目
		//下
		16,18,17,	//三角形1つ目
		17,18,19,	//三角形2つ目
		//上
		20,21,22,	//三角形3つ目
		22,21,23	//三角形4つ目
	};

	PolygonIndexData result;
	result.vertBuffer = GenerateVertexBuffer(lVertices.data(), sizeof(DirectX::XMFLOAT3), lVertices.size());
	result.indexBuffer = GenerateIndexBuffer(lIndices);
	result.index = KazRenderHelper::SetDrawIndexInstanceCommandData(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		KazBufferHelper::SetVertexBufferView(result.vertBuffer->bufferWrapper.GetGpuAddress(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(lVertices.size(), sizeof(DirectX::XMFLOAT3)), sizeof(lVertices[0])),
		KazBufferHelper::SetIndexBufferView(result.indexBuffer->bufferWrapper.GetGpuAddress(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(lIndices.size(), sizeof(USHORT))),
		static_cast<UINT>(lIndices.size()),
		1
	);
	return result;
}

PolygonIndexData PolygonBuffer::GeneratePlaneBuffer(float scale)
{
	std::vector<DirectX::XMFLOAT3> lVertices = GetPlaneVertices({ 0.5f,0.5f }, { scale ,scale });
	std::vector<USHORT> lIndices;
	for (int i = 0; i < 6; ++i)
	{
		lIndices.emplace_back(KazRenderHelper::InitIndciesForPlanePolygon()[i]);
	}

	PolygonIndexData result;
	result.vertBuffer = GenerateVertexBuffer(lVertices.data(), sizeof(DirectX::XMFLOAT3), lVertices.size());
	result.indexBuffer = GenerateIndexBuffer(lIndices);
	result.index = KazRenderHelper::SetDrawIndexInstanceCommandData(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		KazBufferHelper::SetVertexBufferView(result.vertBuffer->bufferWrapper.GetGpuAddress(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(lVertices.size(), sizeof(DirectX::XMFLOAT3)), sizeof(lVertices[0])),
		KazBufferHelper::SetIndexBufferView(result.indexBuffer->bufferWrapper.GetGpuAddress(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(lIndices.size(), sizeof(USHORT))),
		static_cast<UINT>(lIndices.size()),
		1
	);
	return result;
}

PolygonIndexData PolygonBuffer::GeneratePlaneTexBuffer(const KazMath::Vec2<float> &scale)
{
	std::vector<DirectX::XMFLOAT3> lVertices = GetPlaneVertices({ 0.5f,0.5f }, scale);
	std::vector<DirectX::XMFLOAT2> lUv(4);
	KazRenderHelper::InitUvPos(&lUv[0], &lUv[1], &lUv[2], &lUv[3]);
	std::vector<VertUvData> lVertUv;
	for (int i = 0; i < lVertices.size(); ++i)
	{
		lVertUv.emplace_back();
		lVertUv[i].pos = { lVertices[i].x,lVertices[i].y,lVertices[i].z };
		lVertUv[i].uv = lUv[i];
	}

	std::vector<USHORT> lIndices;
	for (int i = 0; i < 6; ++i)
	{
		lIndices.emplace_back(KazRenderHelper::InitIndciesForPlanePolygon()[i]);
	}

	PolygonIndexData result;
	result.vertBuffer = GenerateVertexBuffer(lVertUv.data(), sizeof(VertUvData), lVertUv.size());
	result.indexBuffer = GenerateIndexBuffer(lIndices);
	result.index = KazRenderHelper::SetDrawIndexInstanceCommandData(
		D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		KazBufferHelper::SetVertexBufferView(result.vertBuffer->bufferWrapper.GetGpuAddress(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(lVertices.size(), sizeof(VertUvData)), sizeof(lVertices[0])),
		KazBufferHelper::SetIndexBufferView(result.indexBuffer->bufferWrapper.GetGpuAddress(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(lIndices.size(), sizeof(USHORT))),
		static_cast<UINT>(lIndices.size()),
		1
	);
	return result;
}

std::vector<DirectX::XMFLOAT3> PolygonBuffer::GetPlaneVertices(const KazMath::Vec2<float> &anchorPoint, const KazMath::Vec2<float> &scale)
{
	std::vector<DirectX::XMFLOAT3> vertices(4);
	KazRenderHelper::InitVerticesPos(&vertices[0], &vertices[1], &vertices[2], &vertices[3], anchorPoint.ConvertXMFLOAT2());

	KazMath::Vec2<float>leftUpPos(vertices[0].x, vertices[0].y);
	KazMath::Vec2<float>rightDownPos(vertices[3].x, vertices[3].y);

	return vertices;
}
