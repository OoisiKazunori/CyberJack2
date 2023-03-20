#pragma once
#include"../KazLibrary/Helper/ResouceBufferHelper.h"
#include"../KazLibrary/Render/DrawExcuteIndirect.h"
#include"../KazLibrary/Helper/KazRenderHelper.h"
#include"../KazLibrary/Helper/ISinglton.h"
#include"../KazLibrary/Render/GPUParticleRender.h"

struct InitMeshParticleData
{
	ResouceBufferHelper::BufferData vertData;
	ResouceBufferHelper::BufferData uvData;
	RESOURCE_HANDLE textureHandle;
	//x vertNum, y bias,z perTriangleNum,w faceCountNum
	DirectX::XMUINT4 triagnleData;
	const DirectX::XMMATRIX *motherMat;
	KazMath::Vec4<float>color;
	KazMath::Vec3<float>particleScale;
	bool billboardFlag;
	const float *alpha;

	InitMeshParticleData() :textureHandle(-1), billboardFlag(false)
	{
	}
};

class InstanceMeshParticle
{
public:
	InstanceMeshParticle(const GPUParticleRender *RENDER_PTR);

	void Init();
	void AddMeshData(const InitMeshParticleData &DATA);
	void Compute();


	struct InitOutputData
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		UINT id;
	};

private:


	ResouceBufferHelper computeInitMeshParticle;
	RESOURCE_HANDLE vertHandle, uvHandle, meshDataAndColorHandle, colorHandle, meshParticleOutputHandle, meshParticleIDHandle;
	RESOURCE_HANDLE motherMatrixHandle,particlePosHandle, particleColorHandle,particleMotherMatrixHandle, colorMotherMatrixHandle;
	RESOURCE_HANDLE scaleRotateBillboardMatHandle;

	ResouceBufferHelper computeUpdateMeshParticle;
	ResouceBufferHelper computeConvert;

	ResouceBufferHelper::BufferData commonAndColorBufferData;
	std::vector<ResouceBufferHelper::BufferData> commonBufferData;
	std::vector<ResouceBufferHelper::BufferData> commonColorBufferData;
	ResouceBufferHelper::BufferData meshParticleBufferData;
	struct WorldMatData
	{
		DirectX::XMMATRIX scaleRotateBillboardMat;
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
	static const int VERT_BUFFER_SIZE = sizeof(DirectX::XMFLOAT3);
	static const int UV_BUFFER_SIZE = sizeof(DirectX::XMFLOAT2);
	static const int COMMON_BUFFER_SIZE = sizeof(CommonWithColorData);
	static const int MOTHER_MAT_MAX = 100;

	static int MESH_PARTICLE_GENERATE_NUM;

	struct MotherData
	{
		const DirectX::XMMATRIX *motherMat;
		const float *alpha;
		MotherData(const DirectX::XMMATRIX *M_MAT, const float *ALPHA) :
			motherMat(M_MAT), alpha(ALPHA)
		{
		}
	};

	std::vector<MotherData>motherMatArray;

	enum InitPipelineType
	{
		INIT_POS,
		INIT_POS_UV,
		INIT_POS_UV_NORMAL,
	};
	void IsSetBuffer(const ResouceBufferHelper::BufferData &BUFFER_DATA)
	{
		if (BUFFER_DATA.bufferWrapper.GetBuffer())
		{
			GraphicsRootParamType lType = static_cast<GraphicsRootParamType>(GRAPHICS_PRAMTYPE_DATA + setCountNum);
			computeInitMeshParticle.SetBuffer(BUFFER_DATA, lType);
			++setCountNum;
		}
	};
	int setCountNum;


	DirectX::XMMATRIX scaleRotMat;

	std::vector<InitMeshParticleData> initData;

	KazRenderHelper::ID3D12ResourceWrapper copyBuffer;
	KazRenderHelper::ID3D12ResourceWrapper motherMatrixBuffer;
	KazRenderHelper::ID3D12ResourceWrapper colorBuffer;
	KazRenderHelper::ID3D12ResourceWrapper scaleRotaBuffer;

	struct ScaleRotaBillData
	{
		DirectX::XMMATRIX scaleRotaMata;
		bool billboardFlag;
		ScaleRotaBillData(const DirectX::XMMATRIX &MAT, bool BILLBOARD_FLAG) :
			scaleRotaMata(MAT), billboardFlag(BILLBOARD_FLAG)
		{};
	};
	std::vector<ScaleRotaBillData> scaleRotaMatArray;
};

