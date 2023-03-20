#pragma once
#include"../KazLibrary/Buffer/CreateGpuBuffer.h"
#include"../Game/Interface/IStage.h"
#include<array>
#include<vector>
#include"../Game/Debug/ParameterMgr.h"
#include"../KazLibrary/Helper/ResouceBufferHelper.h"
#include"../KazLibrary/Render/GPUParticleRender.h"

class DeadParticle
{
public:
	DeadParticle(const ResouceBufferHelper::BufferData &ADDRESS, int VERT_NUM, const GPUParticleRender *RENDER_PTR,
		float PARTICLE_SCALE = 0.18f);
	void Init(const DirectX::XMMATRIX *MAT);
	void Update(int ALPHA = 0);
	void Draw();

private:
	bool startFlag;
	const DirectX::XMMATRIX *motherMat;
	int timer;
	//バッファ
	RESOURCE_HANDLE outputInitBufferHandle, outputBufferHandle, counterBufferHandle,
		commonInitBufferHandle, commonBufferHandle;
	BufferMemorySize computeMemSize;
	//バッファ

	int PARTICLE_MAX_NUM = 100000;
	static const int DRAW_CALL = 1;

	struct IndirectCommand
	{
		D3D12_GPU_VIRTUAL_ADDRESS uav;
		D3D12_DRAW_INDEXED_ARGUMENTS drawArguments;
	};


	struct OutputInitData
	{
		DirectX::XMFLOAT4 pos;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT4 vel;
	};

	struct CommonMoveData
	{
		DirectX::XMMATRIX scaleRotateBillboardMat;
		DirectX::XMMATRIX viewProjection;
		DirectX::XMMATRIX motherMat;
	};

	CommonMoveData constBufferData;
	int num;
	DirectX::XMMATRIX scaleRotaMat;

	RESOURCE_HANDLE texHandle;


	ResouceBufferHelper initComputeHelper, updateComputeHelper;

	std::unique_ptr<MeshParticle> meshParticle;

	InitMeshParticleData data;
};