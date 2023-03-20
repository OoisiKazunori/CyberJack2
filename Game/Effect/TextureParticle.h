#pragma once
#include"../KazLibrary/Math/KazMath.h"
#include"../KazLibrary/Buffer/CreateGpuBuffer.h"
#include<array>
#include<vector>
#include"../KazLibrary/Render/KazRender.h"
#include"../KazLibrary/Loader/FbxModelResourceMgr.h"


class TextureParticle
{
public:
	TextureParticle(std::vector<VertexUv> VERT_NUM, const DirectX::XMMATRIX *MOTHER_MAT, RESOURCE_HANDLE HANDLE, float PARTICLE_SCALE = 0.18f, UINT PER_TRIANGLE_COUNT = 50, UINT FACE_COUNT_NUM = 0);
	void Init();
	void Update(bool FLAG = false, bool ENABLE_BILLBOARD_FLAG = true);
	void Draw();

	D3D12_GPU_VIRTUAL_ADDRESS GetAddress()
	{
		return buffers->GetGpuAddress(outputHandle);
	}
	int GetVertNum()
	{
		return constBufferData.vertDataNum.x;
	}
	bool resetSceneFlag;

	struct UpdateCommonData
	{
		DirectX::XMMATRIX scaleRotateBillboardMat;
		DirectX::XMMATRIX viewProjection;
		DirectX::XMMATRIX motherMat;
		DirectX::XMFLOAT2 flash;
		DirectX::XMFLOAT2 pad;
		float alpha;
	};

	UpdateCommonData updateCommonData;
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
	int bias, prevBias;
	float scale;

	RESOURCE_HANDLE tex;

	//必要情報--------------------------------------------
	const DirectX::XMMATRIX *motherMat;
	//必要情報--------------------------------------------

	//初期化--------------------------------------------
	struct OutputData
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT4 baseColor;
	};
	struct InitCommonData
	{
		DirectX::XMFLOAT4 worldPos;
		DirectX::XMUINT4 vertDataNum;
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


	struct UpdateCommonFlashData
	{
		DirectX::XMMATRIX scaleRotateBillboardMat;
		DirectX::XMMATRIX viewProjection;
		DirectX::XMMATRIX motherMat;
		DirectX::XMFLOAT2 flash;
		DirectX::XMFLOAT2 pad;
		float alpha;
	};
	UpdateCommonFlashData updateFlashCommonData;

	RESOURCE_HANDLE updateHandle, updateCommonHandle, updateFlashCommonHandle;
	RESOURCE_HANDLE updateViewHandle;
	DirectX::XMMATRIX scaleRotaMat;


	UINT indexNum;

	//更新用--------------------------------------------

	RESOURCE_HANDLE drawCommandHandle;
};

