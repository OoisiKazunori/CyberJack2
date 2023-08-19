#pragma once
#include"../KazLibrary/Helper/ResouceBufferHelper.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Helper/ISinglton.h"
#include"../KazLibrary/Helper/Compute.h"
#include"InstanceMeshParticleData.h"
#include<vector>

class InstanceMeshParticle
{
public:
	InstanceMeshParticle(const KazBufferHelper::BufferData &arg_outputMat);

	void Init();
	void AddMeshData(const InitMeshParticleData &DATA);
	void Compute();

	void InitCompute();

	struct InitOutputData
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		UINT id;
	};

	KazBufferHelper::ID3D12ResourceWrapper copyBuffer;
private:
	struct CameraMatData
	{
		DirectX::XMMATRIX viewProjMat;
		DirectX::XMMATRIX billboard;
	};
	KazBufferHelper::BufferData cameraMatBuffer;

	KazBufferHelper::BufferData m_outputMatrixBuffer;

	bool isInitFlag;
	ComputeShader computeInitMeshParticle;
	KazBufferHelper::BufferData vertHandle, uvHandle, meshDataAndColorHandle, colorHandle, meshParticleOutputHandle, meshParticleIDHandle;
	KazBufferHelper::BufferData motherMatrixHandle,particlePosHandle, particleColorHandle,particleMotherMatrixHandle, colorMotherMatrixHandle;
	KazBufferHelper::BufferData scaleRotateBillboardMatHandle;

	ComputeShader computeUpdateMeshParticle;
	ComputeShader computeConvert;

	KazBufferHelper::BufferData commonAndColorBufferData;
	std::vector<KazBufferHelper::BufferData> commonBufferData;
	std::vector<KazBufferHelper::BufferData> commonColorBufferData;
	KazBufferHelper::BufferData meshParticleBufferData;
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
	struct MotherMatData
	{
		DirectX::XMMATRIX motherMat;
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
		const bool *curlNozieFlag;
		MotherData(const DirectX::XMMATRIX *arg_motherMatPtr, const float *arg_alphaPtr,const bool *arg_curlNoizeFlagPtr) :
			motherMat(arg_motherMatPtr), alpha(arg_alphaPtr), curlNozieFlag(arg_curlNoizeFlagPtr)
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
	void IsSetBuffer(const KazBufferHelper::BufferData &BUFFER_DATA,std::vector<KazBufferHelper::BufferData>& arg_bufferArray)
	{
		if (BUFFER_DATA.bufferWrapper->GetBuffer())
		{
			GraphicsRootParamType lType = static_cast<GraphicsRootParamType>(GRAPHICS_PRAMTYPE_DATA + setCountNum);
			arg_bufferArray.emplace_back(BUFFER_DATA);
			arg_bufferArray.back().rootParamType = lType;
			++setCountNum;
		}
	};
	int setCountNum;


	DirectX::XMMATRIX scaleRotMat;

	std::vector<InitMeshParticleData> initData;

	KazBufferHelper::ID3D12ResourceWrapper motherMatrixBuffer;
	KazBufferHelper::ID3D12ResourceWrapper colorBuffer;
	KazBufferHelper::ID3D12ResourceWrapper scaleRotaBuffer;
	KazBufferHelper::BufferData curlNoizeUploadBuffer;
	KazBufferHelper::BufferData curlNoizeVRAMBuffer;

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

