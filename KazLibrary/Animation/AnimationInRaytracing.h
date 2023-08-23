#pragma once
#include"../KazLibrary/Helper/Compute.h"

class AnimationInRaytracing
{
public:
	AnimationInRaytracing();

	void Compute(
		const KazBufferHelper::BufferData& arg_vertexBuffer,
		const KazBufferHelper::BufferData& arg_boneBuffer,
		const DirectX::XMMATRIX& arg_worldMat
	);

	void GenerateBuffer(const KazBufferHelper::BufferData& arg_vertexBuffer);

	KazBufferHelper::BufferData m_vertexBuffer;
	KazBufferHelper::BufferData m_indexBuffer;
private:
	ComputeShader m_compute;
	struct CommonData
	{
		DirectX::XMMATRIX m_worldMat;
		DirectX::XMMATRIX m_viewProj;
		UINT vertNum;
	};
};

