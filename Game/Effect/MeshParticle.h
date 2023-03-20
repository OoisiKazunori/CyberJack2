#pragma once
#include"../KazLibrary/Helper/ResouceBufferHelper.h"
#include"../KazLibrary/Render/DrawExcuteIndirect.h"
#include"../KazLibrary/Helper/KazRenderHelper.h"
#include"../Game/Effect/InstanceMeshParticle.h"

/// <summary>
/// メッシュパーティクルの初期化
/// </summary>
class MeshParticle
{
public:
	MeshParticle(const InitMeshParticleData &DATA, UINT ID);

	void Compute();
	const ResouceBufferHelper::BufferData &GetBuffer()
	{
		return particleBuffer;
	}
private:
	ResouceBufferHelper::BufferData particleBuffer;
	ResouceBufferHelper::BufferData commonBufferData;
	ResouceBufferHelper::BufferData commonAndColorBufferData;
	ResouceBufferHelper bufferHelper;

	ComputePipeLineNames pipelineName;


	struct WorldMatData
	{
		DirectX::XMMATRIX scaleRotateBillboardMat;
	};
	struct InitOutputData
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		UINT id;
	};
	struct CommonWithColorData
	{
		DirectX::XMUINT4 meshData;
		DirectX::XMFLOAT4 color;
		UINT id;
	};
	struct CommonData
	{
		DirectX::XMUINT4 meshData;
		UINT id;
	};

	static const int PARTICLE_MAX_NUM = 2000000;
	static const int COMMON_BUFFER_SIZE = sizeof(CommonWithColorData);

	std::vector<const DirectX::XMMATRIX *>motherMatArray;

	enum InitPipelineType
	{
		INIT_POS,
		INIT_POS_UV,
		INIT_POS_UV_NORMAL,
	};

	UINT setCountNum;
	void IsSetBuffer(const ResouceBufferHelper::BufferData &BUFFER_DATA)
	{
		if (BUFFER_DATA.bufferWrapper.GetBuffer())
		{
			GraphicsRootParamType lType = static_cast<GraphicsRootParamType>(GRAPHICS_PRAMTYPE_DATA + setCountNum);
			bufferHelper.SetBuffer(BUFFER_DATA, lType);
			++setCountNum;
		}
	};



	KazRenderHelper::ID3D12ResourceWrapper copyBuffer;
};