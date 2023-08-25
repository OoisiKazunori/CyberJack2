#pragma once
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Math/KazMath.h"

struct InitMeshParticleData
{
	KazBufferHelper::BufferData vertData;
	KazBufferHelper::BufferData uvData;
	KazBufferHelper::BufferData modelVertexBuffer,modelIndexBuffer;
	RESOURCE_HANDLE textureHandle;
	//x vertNum, y bias,z perTriangleNum,w faceCountNum
	DirectX::XMUINT4 triagnleData;
	const DirectX::XMMATRIX* motherMat;
	KazMath::Vec4<float>color;
	KazMath::Vec3<float>particleScale;
	bool billboardFlag;
	const float* alpha;
	const bool* curlNoizeFlag;

	InitMeshParticleData() :textureHandle(-1), billboardFlag(false)
	{
	}
};