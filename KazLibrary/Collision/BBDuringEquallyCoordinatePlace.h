#pragma once
#include"../KazLibrary/Helper/ResouceBufferHelper.h"
#include"BoundingBox.h"

/// <summary>
/// BoundingBoxの情報を元に、等間隔に座標を置く
/// </summary>
class BBDuringEquallyCoordinatePlace
{
public:
	BBDuringEquallyCoordinatePlace(
		const ResouceBufferHelper::BufferData &BB_BUFFER_DATA, 
		const BoundingBoxData &DATA,
		const ResouceBufferHelper::BufferData &STACK_MESH_CIRCLE_DATA,
		float MESH_HITBOX_RADIUS,
		UINT MESH_ID
	);

	void Compute();
	void DebugCompute();
	UINT MaxHitBoxPosNum()
	{
		return countNum;
	}
	const ResouceBufferHelper::BufferData GetHitBoxPosData()
	{
		return computeHelper->GetBufferData(hitBoxDataHandle);
	}

	std::unique_ptr<ResouceBufferHelper> computeHelper;
	ResouceBufferHelper debugComputeHelper;

	//デバック用に当たり判定を描画する用
	void SetDebugDraw(const ResouceBufferHelper::BufferData &STACK_DRAW_DATA);
private:

	D3D12_GPU_DESCRIPTOR_HANDLE bbViewHandle;
	RESOURCE_HANDLE hitBoxDataHandle;
	RESOURCE_HANDLE hitBoxCommonHandle;

	BoundingBoxData data;
	float diameter;
	UINT CalculatingDeployableNumber(float DISTANCE, float RADIUS);

	struct HitBoxConstBufferData
	{
		float diameter;
		UINT id;
		UINT xMax;
		UINT xyMax;
	};

	struct MeshHitBoxData
	{
		DirectX::XMFLOAT3 pos;
		UINT meshID;
		DirectX::XMUINT3 id;
	};

	DispatchCallData threadNumData;
	UINT countNum;

	bool debugFlag;
};
