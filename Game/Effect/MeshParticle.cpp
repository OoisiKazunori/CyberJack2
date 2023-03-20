#include"MeshParticle.h"

MeshParticle::MeshParticle(const InitMeshParticleData &DATA, UINT ID)
{
	setCountNum = 0;

#pragma region 初期化用のバッファ生成

	commonAndColorBufferData = bufferHelper.CreateAndGetBuffer(
		COMMON_BUFFER_SIZE,
		GRAPHICS_RANGE_TYPE_CBV_VIEW,
		GRAPHICS_PRAMTYPE_DATA,
		1,
		false
	);

	commonBufferData = bufferHelper.CreateAndGetBuffer(
		sizeof(CommonData),
		GRAPHICS_RANGE_TYPE_CBV_VIEW,
		GRAPHICS_PRAMTYPE_DATA,
		1,
		false
	);


	//何の情報を読み込むかでパイプラインの種類を変える
	IsSetBuffer(DATA.vertData);
	IsSetBuffer(DATA.uvData);


	pipelineName = PIPELINE_COMPUTE_NAME_NONE;
	RESOURCE_HANDLE lCommonHandle;
	CommonWithColorData lCommonAndColorData;
	CommonData lCommonData;

	switch (setCountNum)
	{
	case 0:
		//メッシュパーティクルに必要な情報が何も入ってない
		assert(0);
		break;
	case 1:
		lCommonHandle = bufferHelper.SetBuffer(commonAndColorBufferData, GRAPHICS_PRAMTYPE_DATA3);

		lCommonAndColorData.meshData = DATA.triagnleData;
		lCommonAndColorData.color = DATA.color.ConvertXMFLOAT4();
		lCommonAndColorData.id = ID;
		bufferHelper.TransData(lCommonHandle, &lCommonAndColorData, sizeof(CommonWithColorData));

		pipelineName = PIPELINE_COMPUTE_NAME_INIT_POS_MESHPARTICLE;
		break;
	case 2:
		lCommonHandle = bufferHelper.SetBuffer(commonBufferData, GRAPHICS_PRAMTYPE_DATA4);

		lCommonData.meshData = DATA.triagnleData;
		lCommonData.id = ID;
		bufferHelper.TransData(lCommonHandle, &lCommonData, sizeof(CommonData));

		pipelineName = PIPELINE_COMPUTE_NAME_INIT_POSUV_MESHPARTICLE;
		break;
	default:
		break;
	}

	//テクスチャのセット
	if (DATA.textureHandle != -1)
	{
		ResouceBufferHelper::BufferData lData;
		lData.CreateViewHandle(DATA.textureHandle);
		lData.rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
		lData.rootParamType = GRAPHICS_PRAMTYPE_TEX;
		bufferHelper.SetBuffer(lData, GRAPHICS_PRAMTYPE_TEX);
	}

	GraphicsRootParamType lRootParam = static_cast<GraphicsRootParamType>(GRAPHICS_PRAMTYPE_DATA + setCountNum);
	particleBuffer = bufferHelper.CreateAndGetBuffer(
		KazBufferHelper::SetOnlyReadStructuredBuffer(sizeof(InitOutputData) * PARTICLE_MAX_NUM),
		GRAPHICS_RANGE_TYPE_UAV_DESC,
		lRootParam,
		sizeof(InitOutputData),
		PARTICLE_MAX_NUM,
		true);

	//パーティクル情報の詰め込み
	bufferHelper.SetBuffer(particleBuffer, lRootParam);
#pragma endregion


	UINT lNum = 0;
	KazBufferHelper::BufferResourceData lBufferData
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		"CopyCounterBuffer"
	);
	copyBuffer.CreateBuffer(lBufferData);
	copyBuffer.TransData(&lNum, sizeof(UINT));

	bufferHelper.InitCounterBuffer(copyBuffer.GetBuffer());
	bufferHelper.StackToCommandListAndCallDispatch(pipelineName, { 1000,1,1 });
}

void MeshParticle::Compute()
{
	bufferHelper.StackToCommandListAndCallDispatch(pipelineName, { 1000,1,1 });
}
