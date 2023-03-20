#pragma once
#include"../KazLibrary/Math/KazMath.h"
#include"../KazLibrary/Buffer/CreateGpuBuffer.h"
#include<array>
#include<vector>
#include"../KazLibrary/Render/KazRender.h"

class GPUCollisionDetectionOfMeshHitBoxAndSphere
{
public:
	GPUCollisionDetectionOfMeshHitBoxAndSphere(std::vector<DirectX::XMFLOAT4> VERT_NUM, float PARTICLE_SCALE = 0.18f);
	~GPUCollisionDetectionOfMeshHitBoxAndSphere();
	void Init(const DirectX::XMMATRIX *MOTHER_MAT);
	void Update(RESOURCE_HANDLE BASE_HANDLE, RESOURCE_HANDLE PARTICLE_HANDLE, bool DIRTY_FLAG);
	void Draw();

private:

	int sceneNum;
	//バッファ
	std::unique_ptr<CreateGpuBuffer> buffers;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	//バッファ

	static const int PARTICLE_MAX_NUM = 100000;
	static const int DRAW_CALL = 1;

	struct IndirectCommand
	{
		D3D12_GPU_VIRTUAL_ADDRESS uav;
		D3D12_DRAW_INDEXED_ARGUMENTS drawArguments;
	};



	Microsoft::WRL::ComPtr<ID3D12CommandSignature> commandSig;
	bool drawParticleFlag;
	float scale;

	//必要情報--------------------------------------------
	KazMath::Vec3<float> *pos;
	const DirectX::XMMATRIX *motherMat;
	//必要情報--------------------------------------------

	//初期化--------------------------------------------
	struct OutputData
	{
		DirectX::XMFLOAT3 trianglePos[3];
		DirectX::XMFLOAT3 normal;
	};
	struct InitCommonData
	{
		DirectX::XMFLOAT4 worldPos;
	};
	InitCommonData constBufferData;
	RESOURCE_HANDLE vertexBufferHandle, indexBufferHandle;
	RESOURCE_HANDLE verticesDataHandle, initCommonHandle;

	RESOURCE_HANDLE outputHandle;

	RESOURCE_HANDLE outputViewHandle, vertDataViewHandle;
	//初期化--------------------------------------------

	//更新用--------------------------------------------
	struct UpdateData
	{
		DirectX::XMMATRIX matrix;
		DirectX::XMFLOAT4 color;
	};
	struct UpdateCommonData
	{
		DirectX::XMMATRIX scaleRotateBillboardMat;
		DirectX::XMMATRIX viewProjection;
		DirectX::XMFLOAT4 pos;
		float radius;
		UINT meshNum;
		UINT hitVel;
	};
	struct MeshDrawCommonData
	{
		DirectX::XMMATRIX scaleRotateBillboardMat;
		DirectX::XMMATRIX viewProjection;
		DirectX::XMMATRIX motherMat;
	};
	UpdateCommonData updateCommonData;
	MeshDrawCommonData drawMeshCommonData;
	RESOURCE_HANDLE updateHandle, updateCommonHandle, drawMeshCommonHandle;
	RESOURCE_HANDLE updateViewHandle;
	DirectX::XMMATRIX scaleRotaMat;

	RESOURCE_HANDLE updateHitboxHandle, updateHitboxViewHandle;

	UINT indexNum;

	//更新用--------------------------------------------

	RESOURCE_HANDLE drawCommandHandle;

	FbxModelRender pointPos;
	int hitVel;
};

