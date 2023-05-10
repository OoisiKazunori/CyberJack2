#include"Polygon.h"

std::shared_ptr<KazBufferHelper::BufferData> PolygonBuffer::GenerateVertexBuffer(void *verticesPos, int structureSize, size_t arraySize)
{
	BUFFER_SIZE lSize = static_cast<BUFFER_SIZE>(arraySize * structureSize);
	//RAM�̃o�b�t�@�Ɉ�x�]��---------------------------------------
	cpuVertBuffer = std::make_shared<KazBufferHelper::BufferData>(KazBufferHelper::SetVertexBufferData(lSize));
	cpuVertBuffer->bufferWrapper.TransData(verticesPos, lSize);
	//RAM�̃o�b�t�@�Ɉ�x�]��---------------------------------------

	//VRAM�̃o�b�t�@�ɃR�s�[---------------------------------------
	vertBuffer = std::make_shared<KazBufferHelper::BufferData>(KazBufferHelper::SetGPUBufferData(lSize));
	vertBuffer->bufferWrapper.CopyBuffer(cpuVertBuffer->bufferWrapper.GetBuffer(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST);
	//VRAM�̃o�b�t�@�ɃR�s�[---------------------------------------

	return cpuVertBuffer;
}

std::shared_ptr<KazBufferHelper::BufferData> PolygonBuffer::GenerateIndexBuffer(std::vector<USHORT> indices)
{
	BUFFER_SIZE lSize = static_cast<BUFFER_SIZE>(indices.size() * sizeof(USHORT));
	//RAM�̃o�b�t�@�Ɉ�x�]��---------------------------------------
	cpuIndexBuffer = std::make_shared<KazBufferHelper::BufferData>(KazBufferHelper::SetIndexBufferData(lSize));
	cpuIndexBuffer->bufferWrapper.TransData(indices.data(), lSize);
	//RAM�̃o�b�t�@�Ɉ�x�]��---------------------------------------

	//VRAM�̃o�b�t�@�ɃR�s�[---------------------------------------
	//indexBuffer = std::make_shared<KazBufferHelper::BufferData>(KazBufferHelper::SetGPUBufferData(lSize));
	//indexBuffer->bufferWrapper.CopyBuffer(cpuIndexBuffer->bufferWrapper.GetBuffer(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST);
	//VRAM�̃o�b�t�@�ɃR�s�[---------------------------------------

	return cpuIndexBuffer;
}

PolygonIndexData PolygonBuffer::GenerateBoxBuffer(float scale)
{
	const float lSize = scale;

	std::vector<DirectX::XMFLOAT3> lVertices =
	{
	//�O
	{-lSize, -lSize, -lSize},	//����
	{-lSize,  lSize, -lSize},		//����
	{ lSize, -lSize, -lSize},		//�E��
	{ lSize,  lSize, -lSize},		//�E��

	//��
	{-lSize,-lSize,  lSize}, 	//����
	{-lSize, lSize,  lSize}, 	//����
	{ lSize,-lSize,  lSize},	//�E��
	{ lSize, lSize,  lSize},	//�E��

	//��
	{-lSize,-lSize, -lSize},	//����
	{-lSize,-lSize,  lSize}, 	//����
	{-lSize, lSize, -lSize}, 	//�E��
	{-lSize, lSize,  lSize}, 	//�E��

	//�E
	{lSize,-lSize, -lSize},	//����
	{lSize,-lSize,  lSize},	//����
	{lSize, lSize, -lSize},	//�E��
	{lSize, lSize,  lSize},	//�E��

	//��
	{ lSize, -lSize, lSize}, 	//����
	{ lSize, -lSize,-lSize}, 	//����
	{-lSize, -lSize, lSize}, 	//�E��
	{-lSize, -lSize,-lSize}, //�E��

	//��
	{ lSize, lSize, lSize},		//����
	{ lSize, lSize,-lSize},		//����
	{-lSize, lSize, lSize},		//�E��
	{-lSize, lSize,-lSize}	//�E��
	};

	std::vector<USHORT> lIndices =
	{
		//�O
		0,1,2,		//�O�p�`1��
		2,1,3,		//�O�p�`2��
		//��
		4,6,5,		//�O�p�`3��
		6,7,5,		//�O�p�`4��
		//��
		8,9,10,		//�O�p�`1��
		10,9,11,	//�O�p�`2��
		//�E
		12,14,13,	//�O�p�`1��
		13,14,15,	//�O�p�`2��
		//��
		16,18,17,	//�O�p�`1��
		17,18,19,	//�O�p�`2��
		//��
		20,21,22,	//�O�p�`3��
		22,21,23	//�O�p�`4��
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
