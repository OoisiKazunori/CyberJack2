#pragma once
#include"../KazLibrary/Helper/ResouceBufferHelper.h"

struct BoundingBoxData
{
	KazMath::Vec3<float> minPos;
	KazMath::Vec3<float> maxPos;
};

/// <summary>
/// ì«Ç›çûÇÒÇæÉÇÉfÉãÇ©ÇÁBoundingBoxÇå`ê¨Ç∑ÇÈèÓïÒÇçÏÇÈ
/// </summary>
class BoundingBox
{
public:
	BoundingBox(const ResouceBufferHelper::BufferData &VERT_DATA, UINT VERT_MAX_NUM);
	RESOURCE_HANDLE GetViewHandle();

	const ResouceBufferHelper::BufferData &GetBBBuffer();
	BoundingBoxData GetData(int BB_INDEX = -1);
	void Compute();
	
	ResouceBufferHelper computeBuffer;
private:
	struct BoundingBoxBufferData
	{
		DirectX::XMFLOAT3 minPos;
		DirectX::XMFLOAT3 maxPos;
	};
	struct CommonData
	{
		DirectX::XMMATRIX scaleRotaMat;
		UINT vertNum;
	};


	RESOURCE_HANDLE bbBufferHandle;
	RESOURCE_HANDLE vertBufferHandle;
	RESOURCE_HANDLE matBufferHandle;

};