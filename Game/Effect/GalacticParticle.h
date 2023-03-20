#pragma once
#include"../KazLibrary/Buffer/CreateGpuBuffer.h"
#include"../Game/Interface/IStage.h"
#include<array>
#include<vector>
#include"../Game/Debug/ParameterMgr.h"

class GalacticParticle
{
public:
	GalacticParticle();
	~GalacticParticle();
	void Update();
	void Draw();

	std::vector<Sphere> blockHitBox;

private:
	//バッファ
	std::unique_ptr<CreateGpuBuffer> buffers;
	RESOURCE_HANDLE vertexBufferHandle, indexBufferHandle,
		outputInitBufferHandle, outputBufferHandle,
		drawCommandHandle, counterBufferHandle,
		commonInitBufferHandle, commonBufferHandle;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	BufferMemorySize computeMemSize;
	RESOURCE_HANDLE outputInitViewHandle, outputViewHandle;
	//バッファ

	static const int PARTICLE_MAX_NUM = 20000;
	static const int DRAW_CALL = 1;

	struct IndirectCommand
	{
		D3D12_GPU_VIRTUAL_ADDRESS uav;
		D3D12_DRAW_INDEXED_ARGUMENTS drawArguments;
	};


	struct OutputInitData
	{
		DirectX::XMFLOAT4 pos;
	};

	struct OutputData
	{
		DirectX::XMMATRIX mat;
		DirectX::XMFLOAT4 color;
	};

	struct CommonMoveData
	{
		DirectX::XMMATRIX cameraMat;
		DirectX::XMMATRIX projectionMat;
		DirectX::XMMATRIX billboardMat;
	};

	Microsoft::WRL::ComPtr<ID3D12CommandSignature> commandSig;
	CommonMoveData constBufferData;
	int num;

	ParameterMgr blockFileMgr;

	BoxPolygonRenderPtr box;
	RESOURCE_HANDLE instanceBufferHandle;

	struct BoxData
	{
		KazMath::Transform3D transform;
		KazMath::Vec3<float> vel;
		KazMath::Vec3<float> rotaVel;
		KazMath::Color color;

		BoxData() :color(255, 255, 255, 255)
		{};
	};
	static const int BOX_MAX_NUM = 50;
	std::array<BoxData, BOX_MAX_NUM> boxDataArray;

	struct MatData
	{
		DirectX::XMFLOAT4 color;
		DirectX::XMMATRIX mat;
	};

	RESOURCE_HANDLE texHandle;




	static const int HITBOX_MAX_NUM = 25;
	std::array<BoxData, HITBOX_MAX_NUM> hitStageArray;
	std::array<BoxPolygonRender, HITBOX_MAX_NUM>hitRender;
};

