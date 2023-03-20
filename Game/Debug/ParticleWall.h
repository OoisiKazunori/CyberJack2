#pragma once
#include"../KazLibrary/Buffer/CreateGpuBuffer.h"
#include<array>
#include<vector>

class ParticleWall
{
public:
	ParticleWall();
	void Update();
	void Draw();

	RESOURCE_HANDLE GetBasePosHandle()
	{
		return baseOutputInitViewHandle;
	};
	RESOURCE_HANDLE GetParticleHandle()
	{
		return outputInitViewHandle;
	};
private:
	//バッファ
	std::unique_ptr<CreateGpuBuffer> buffers;
	RESOURCE_HANDLE vertexBufferHandle, indexBufferHandle,
		outputInitBufferHandle, baseOutputInitBufferHandle, outputBufferHandle,
		particleDataHandle, drawCommandHandle, counterBufferHandle,
		commonInitBufferHandle, commonBufferHandle;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	BufferMemorySize computeMemSize;
	RESOURCE_HANDLE outputInitViewHandle, baseOutputInitViewHandle,outputViewHandle, particleDataViewHandle;
	//バッファ

	static const int PARTICLE_MAX_NUM = 4000;
	static const int PER_USE_PARTICLE_MAX_NUM = 200;
	static const int DRAW_CALL = 1;

	struct IndirectCommand
	{
		D3D12_GPU_VIRTUAL_ADDRESS uav;
		D3D12_DRAW_INDEXED_ARGUMENTS drawArguments;
	};

	struct ParticleData
	{
		DirectX::XMFLOAT4 pos;
	};

	struct OutputInitData
	{
		DirectX::XMFLOAT4 pos;
		DirectX::XMFLOAT4 color;
	};

	struct OutputData
	{
		DirectX::XMMATRIX mat;
		DirectX::XMFLOAT4 color;
	};

	struct CommonData
	{
		DirectX::XMMATRIX cameraMat;
		DirectX::XMMATRIX projectionMat;
		DirectX::XMMATRIX billboardMat;
		DirectX::XMFLOAT4 vertices[8];
	};


	struct CommonMoveData
	{
		DirectX::XMMATRIX viewProjectionMat;
		DirectX::XMMATRIX scaleRotateBillboardMat;
		DirectX::XMFLOAT2 flash;
	};

	Microsoft::WRL::ComPtr<ID3D12CommandSignature> commandSig;
	CommonMoveData constBufferData;
	int num;
};

