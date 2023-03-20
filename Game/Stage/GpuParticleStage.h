#pragma once
#include"../KazLibrary/Buffer/CreateGpuBuffer.h"
#include"../Game/Interface/IStage.h"
#include<array>
#include<vector>

class GpuParticleStage :public IStage
{
public:
	GpuParticleStage();
	~GpuParticleStage();
	void Update()override;
	void Draw()override;

private:
	//バッファ
	std::unique_ptr<CreateGpuBuffer> buffers;
	RESOURCE_HANDLE vertexBufferHandle, indexBufferHandle, outputInitBufferHandle, particleDataHandle, drawCommandHandle, counterBufferHandle, commonInitBufferHandle;
	RESOURCE_HANDLE texHandle;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	BufferMemorySize computeMemSize;
	//バッファ

	static const int PARTICLE_MAX_NUM = 40000;
	static const int DRAW_CALL = 1;

	struct IndirectCommand
	{
		D3D12_GPU_VIRTUAL_ADDRESS uav;
		D3D12_DRAW_INDEXED_ARGUMENTS drawArguments;
	};

	struct ParticleData
	{
		DirectX::XMFLOAT4 pos;
		DirectX::XMFLOAT4 vel;
		DirectX::XMFLOAT4 color;
		UINT timer;
	};

	struct OutputInitData
	{
		DirectX::XMMATRIX mat;
		DirectX::XMFLOAT4 color;
	};

	struct CommonData
	{
		DirectX::XMMATRIX cameraMat;
		DirectX::XMMATRIX projectionMat;
		DirectX::XMMATRIX billboardMat;
		DirectX::XMFLOAT4 emittPos;
		UINT increSize;
		UINT64 gpuAddress;
		int seed;
	};

	Microsoft::WRL::ComPtr<ID3D12CommandSignature> commandSig;

};