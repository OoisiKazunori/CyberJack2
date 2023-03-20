#pragma once
#include"../KazLibrary/Helper/ResouceBufferHelper.h"
#include"../KazLibrary/Render/DrawExcuteIndirect.h"
#include"../KazLibrary/Helper/KazRenderHelper.h"
#include"../KazLibrary/Helper/ISinglton.h"
#include"../KazLibrary/Render/GPUParticleRender.h"
#include"../Game/Effect/MeshParticle.h"

struct InitDeadParticleData
{
	const bool *startFlag;
	InitMeshParticleData meshParticleData;
	const DirectX::XMMATRIX *motherMat;
};

class InstanceDeadParticle
{
public:
	InstanceDeadParticle(const GPUParticleRender *RENDER_PTR);
	void Init();
	void AddData(const InitDeadParticleData &INIT_DATA);
	void Compute();
private:
	static const int PARTICLE_MAX_NUM = 1000000;
	static int meshIdNum;

	ResouceBufferHelper initComputeHelper;
	ResouceBufferHelper updateComputeHelper;

	struct DeadparticleData
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT3 vel;
		UINT meshID;
	};

	RESOURCE_HANDLE appendParticleBufferHandle;
	RESOURCE_HANDLE scaleRotateBillboardMatHandle;
	RESOURCE_HANDLE motherMatHandle, startFlagHandle;

	struct CommonData
	{
		UINT id;
		UINT vertNum;
	};
	std::vector<ResouceBufferHelper::BufferData> idBufferData;
	std::vector<MeshParticle> meshParticleData;
	KazRenderHelper::ID3D12ResourceWrapper motherMatBuffer;

	struct MotherData
	{
		const DirectX::XMMATRIX *motherMat;
		const bool *startFlag;
		MotherData(const DirectX::XMMATRIX *MAT, const bool *FLAG) :motherMat(MAT), startFlag(FLAG)
		{};
	};
	struct MotherBufferData
	{
		DirectX::XMMATRIX motherMat;
		UINT startFlag;
	};
	std::vector<MotherData>matArray;


	KazRenderHelper::ID3D12ResourceWrapper copyBuffer;
};

