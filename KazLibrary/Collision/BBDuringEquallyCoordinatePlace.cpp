#include "BBDuringEquallyCoordinatePlace.h"
#include"../KazLibrary/DirectXCommon/DirectX12Device.h"
#include"../KazLibrary/DirectXCommon/DirectX12CmdList.h"
#include"../KazLibrary/Pipeline/GraphicsRootSignature.h"
#include"../KazLibrary/Pipeline/GraphicsPipeLineMgr.h"
#include"../KazLibrary/Helper/KazRenderHelper.h"
#include"../KazLibrary/RenderTarget/RenderTargetStatus.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../KazLibrary/Loader/ObjResourceMgr.h"
#include"../KazLibrary/Buffer/UavViewHandleMgr.h"

BBDuringEquallyCoordinatePlace::BBDuringEquallyCoordinatePlace(const ResouceBufferHelper::BufferData &BB_BUFFER_DATA, const BoundingBoxData &DATA, const ResouceBufferHelper::BufferData &STACK_MESH_CIRCLE_DATA, float MESH_HITBOX_RADIUS,UINT MESH_ID) :data(DATA), countNum(0), debugFlag(false), diameter(MESH_HITBOX_RADIUS)
{
	computeHelper = std::make_unique<ResouceBufferHelper>();

	//いくつ配置出来るか計算する
	KazMath::Vec3<float>lDistance = data.maxPos - data.minPos;

	threadNumData.x = CalculatingDeployableNumber(lDistance.x, diameter);
	threadNumData.y = CalculatingDeployableNumber(lDistance.y, diameter);
	threadNumData.z = CalculatingDeployableNumber(lDistance.z, diameter);

	countNum = static_cast<BUFFER_SIZE>(threadNumData.x * threadNumData.y * threadNumData.z);

	computeHelper->SetBuffer(BB_BUFFER_DATA, GRAPHICS_PRAMTYPE_DATA);

	//当たり判定座標の用意
	hitBoxDataHandle = computeHelper->CreateBuffer(
		KazBufferHelper::SetOnlyReadStructuredBuffer(sizeof(MeshHitBoxData) * countNum),
		GRAPHICS_RANGE_TYPE_UAV_DESC,
		GRAPHICS_PRAMTYPE_DATA2,
		sizeof(MeshHitBoxData),
		countNum
	);

	//メッシュ球のスタック情報
	computeHelper->SetBuffer(STACK_MESH_CIRCLE_DATA, GRAPHICS_PRAMTYPE_DATA3);


	//事前に計算しておくもの用意
	{
		RESOURCE_HANDLE lHandle = computeHelper->CreateBuffer(sizeof(HitBoxConstBufferData), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA4, 1);
		HitBoxConstBufferData lData;
		lData.diameter = diameter;
		lData.xMax = threadNumData.x;
		lData.xyMax = threadNumData.x * threadNumData.y;
		lData.id = MESH_ID;
		computeHelper->TransData(lHandle, &lData, sizeof(HitBoxConstBufferData));
	}


	//デバック描画
	{
		debugComputeHelper.SetBuffer(BB_BUFFER_DATA, GRAPHICS_PRAMTYPE_DATA);
		debugComputeHelper.SetBuffer(computeHelper->GetBufferData(hitBoxDataHandle), GRAPHICS_PRAMTYPE_DATA2);
		debugComputeHelper.SetBuffer(STACK_MESH_CIRCLE_DATA, GRAPHICS_PRAMTYPE_DATA3);
		hitBoxCommonHandle = debugComputeHelper.CreateBuffer(sizeof(HitBoxConstBufferData), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA4, 1);
		HitBoxConstBufferData lData;
		lData.diameter = diameter;
		lData.xMax = threadNumData.x;
		lData.xyMax = threadNumData.x * threadNumData.y;
		lData.id = MESH_ID;
		debugComputeHelper.TransData(hitBoxCommonHandle, &lData, sizeof(HitBoxConstBufferData));
	}
}

void BBDuringEquallyCoordinatePlace::Compute()
{
	computeHelper->StackToCommandListAndCallDispatch(PIPELINE_COMPUTE_NAME_HITBOX_SETCIRCLE_IN_BB, threadNumData);
}

void BBDuringEquallyCoordinatePlace::DebugCompute()
{
	debugComputeHelper.StackToCommandListAndCallDispatch(PIPELINE_COMPUTE_NAME_HITBOX_SETCIRCLE_IN_BB_DEBUG, threadNumData);
}

void BBDuringEquallyCoordinatePlace::SetDebugDraw(const ResouceBufferHelper::BufferData &STACK_DRAW_DATA)
{
	debugComputeHelper.SetBuffer(
		STACK_DRAW_DATA,
		GRAPHICS_PRAMTYPE_DATA4
	);

	debugComputeHelper.SetRootParam(
		hitBoxCommonHandle,
		GRAPHICS_PRAMTYPE_DATA5
	);

	debugFlag = true;
}

UINT BBDuringEquallyCoordinatePlace::CalculatingDeployableNumber(float DISTANCE, float RADIUS)
{
	float lDistance = DISTANCE;
	UINT num = 0;
	while (0.0f < lDistance)
	{
		lDistance -= RADIUS;
		++num;
	}
	return num;
}
