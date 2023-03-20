#pragma once
#include"../KazLibrary/Helper/ResouceBufferHelper.h"
#include"../KazLibrary/Collision/CollisionManager.h"

/// <summary>
/// BB内に配置された当たり判定座標とCPUの当たり判定座標との当たり判定を行います
/// </summary>
class CollisionDetectionOfMeshCircleAndCPUHitBox
{
public:
	CollisionDetectionOfMeshCircleAndCPUHitBox(
		const std::vector<Sphere*> &CPU_HITBOX_DATA,
		float MESH_CIRCLE_RADIUS,
		int MESH_CIRCLE_NUM
	);
	void Compute();

	void SetStackMeshCircleBuffer(const ResouceBufferHelper::BufferData &BUFFER_DATA);
	const ResouceBufferHelper::BufferData &GetStackIDBuffer();

private:
	ResouceBufferHelper computeHelper;
	RESOURCE_HANDLE cpuHitBoxBufferHandle, meshCircleArrayBufferHandle, hitIdBufferHandle;
	RESOURCE_HANDLE commonDataHandle;

	struct CommonData
	{
		UINT cpuHitBoxNum;
		float particleRadius;
		UINT meshCircleNum;
	};
	struct HitIDData
	{
		UINT meshID;
		DirectX::XMUINT3 id;
	};
	struct SphereData
	{
		DirectX::XMFLOAT3 pos;
		float radius;
	};

	struct MeshHitBoxData
	{
		DirectX::XMFLOAT3 meshPos;
		UINT meshID;
		DirectX::XMUINT3 id;
		DirectX::XMFLOAT3 circlePos;
	};
	const std::vector<Sphere*> &sphereHitBoxArray;

	KazRenderHelper::ID3D12ResourceWrapper motherMatrixBuffer;

	KazRenderHelper::ID3D12ResourceWrapper copyBuffer;
};

