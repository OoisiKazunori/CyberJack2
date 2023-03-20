#include "BoundingBox.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"

BoundingBox::BoundingBox(const ResouceBufferHelper::BufferData &VERT_DATA, UINT VERT_MAX_NUM)
{
	computeBuffer.SetBuffer(VERT_DATA, GRAPHICS_PRAMTYPE_DATA);

	//BBを形成する処理用意
	bbBufferHandle = computeBuffer.CreateBuffer(
		sizeof(BoundingBoxBufferData),
		GRAPHICS_RANGE_TYPE_UAV_DESC,
		GRAPHICS_PRAMTYPE_DATA2,
		1);

	//モデルのTransform指定
	matBufferHandle = computeBuffer.CreateBuffer(
		sizeof(CommonData), 
		GRAPHICS_RANGE_TYPE_CBV_VIEW, 
		GRAPHICS_PRAMTYPE_DATA3, 1
	);
	DirectX::XMMATRIX lMat = KazMath::CaluWorld(KazMath::Transform3D({ 0.0f,0.0f,0.0f }, { 10.0f,10.0f,10.0f }, { 0.0f,0.0f,0.0f }), { 0.0f,1.0f,0.0f }, { 0.0f, 0.0f, 1.0f });

	CommonData lData;
	lData.scaleRotaMat = lMat;
	lData.vertNum = VERT_MAX_NUM;
	computeBuffer.TransData(matBufferHandle, &lData, sizeof(CommonData));
}

BoundingBoxData BoundingBox::GetData(int BB_INDEX)
{
	BoundingBoxBufferData *lData = (BoundingBoxBufferData *)computeBuffer.GetBufferData(bbBufferHandle).bufferWrapper.GetMapAddres(BB_INDEX);

	BoundingBoxData lTmpData;
	lTmpData.minPos = { lData->minPos.x, lData->minPos.y, lData->minPos.z };
	lTmpData.maxPos = { lData->maxPos.x, lData->maxPos.y, lData->maxPos.z };
	return lTmpData;
}

const ResouceBufferHelper::BufferData &BoundingBox::GetBBBuffer()
{
	return computeBuffer.GetBufferData(bbBufferHandle);
}

RESOURCE_HANDLE BoundingBox::GetViewHandle()
{
	return computeBuffer.GetBufferData(bbBufferHandle).GetViewHandle();
}

void BoundingBox::Compute()
{
	computeBuffer.StackToCommandListAndCallDispatch(PIPELINE_COMPUTE_NAME_HITBOX_BB, { 3,1,1 });
}