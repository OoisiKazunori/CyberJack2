#pragma once
#include"../KazLibrary/Helper/Compute.h"

class AnimationInRaytracing
{
public:
	AnimationInRaytracing();

	void Compute(
	const KazBufferHelper::BufferData& arg_vertexBuffer,
	const KazBufferHelper::BufferData& arg_boneBuffer,
	const KazBufferHelper::BufferData& arg_indexBuffer
	);

	KazBufferHelper::BufferData m_vertexBuffer;
	KazBufferHelper::BufferData m_indexBuffer;
private:
	ComputeShader m_compute;
};

