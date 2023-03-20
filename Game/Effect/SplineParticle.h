#pragma once
#include"../KazLibrary/Math/KazMath.h"
#include"../KazLibrary/Buffer/CreateGpuBuffer.h"
#include<array>
#include<vector>
#include"../KazLibrary/Render/KazRender.h"
#include"../KazLibrary/Loader/FbxModelResourceMgr.h"

class SplineParticle
{
public:
	SplineParticle(float PARTICLE_SCALE = 0.18f);
	void Init(const std::vector<KazMath::Vec3<float>> &LIMIT_POS_ARRAY, bool APPEAR_FLAG);
	void Update();
	void Draw();

	D3D12_GPU_VIRTUAL_ADDRESS GetAddress()
	{
		return buffers->GetGpuAddress(outputHandle);
	}
	int GetVertNum()
	{
		return -1;
	}
	bool resetSceneFlag;


private:
	int sceneNum;
	//バッファ
	std::unique_ptr<CreateGpuBuffer> buffers;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	//バッファ

	static const int PARTICLE_MAX_NUM = 100000;
	static const UINT LIMITPOS_MAX_NUM = 152;
	static const UINT INIT_LIMITPOS_MAX_NUM = LIMITPOS_MAX_NUM / 2;
	static const int DRAW_CALL = 1;

	struct IndirectCommand
	{
		D3D12_GPU_VIRTUAL_ADDRESS uav;
		D3D12_DRAW_INDEXED_ARGUMENTS drawArguments;
	};



	Microsoft::WRL::ComPtr<ID3D12CommandSignature> commandSig;
	float scale;

	//必要情報--------------------------------------------
	KazMath::Vec3<float> *pos;
	const DirectX::XMMATRIX *motherMat;
	//必要情報--------------------------------------------

	//初期化--------------------------------------------
	struct OutputData
	{
		UINT startIndex;
		float rate;
		float rateVel;
		DirectX::XMFLOAT2 offset;
		DirectX::XMFLOAT4 color;
		float radius;
		float radiusRate;
	};
	struct InitCommonData
	{
		UINT initMaxIndex;
	};
	InitCommonData constBufferData;
	RESOURCE_HANDLE vertexBufferHandle, indexBufferHandle;
	RESOURCE_HANDLE initCommonHandle;

	RESOURCE_HANDLE outputHandle;

	RESOURCE_HANDLE outputViewHandle;
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
	};
	struct UpdateLimitNumData
	{
		UINT limitIndexMaxNum;
	};

	UpdateCommonData updateCommonData;
	RESOURCE_HANDLE updateHandle, updateCommonHandle,updateLimitDataHandle,updateLimitPosDataHandle;
	RESOURCE_HANDLE updateViewHandle,updateLimitPosViewHandle;
	DirectX::XMMATRIX scaleRotaMat;


	UINT indexNum;

	//更新用--------------------------------------------

	RESOURCE_HANDLE drawCommandHandle;

	UpdateLimitNumData limitNumData;
};

