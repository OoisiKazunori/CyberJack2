#include "InstanceMeshParticle.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"

int InstanceMeshParticle::MESH_PARTICLE_GENERATE_NUM = 0;

InstanceMeshParticle::InstanceMeshParticle(const GPUParticleRender *RENDER_PTR) :setCountNum(0)
{
	//メッシュパーティクルの初期化処理の出力情報
	meshParticleBufferData = computeInitMeshParticle.CreateAndGetBuffer(
		KazBufferHelper::SetOnlyReadStructuredBuffer(sizeof(InitOutputData) * PARTICLE_MAX_NUM),
		GRAPHICS_RANGE_TYPE_UAV_DESC,
		GRAPHICS_PRAMTYPE_DATA,
		sizeof(InitOutputData),
		PARTICLE_MAX_NUM,
		true);

	//パーティクルデータ
	computeUpdateMeshParticle.SetBuffer(meshParticleBufferData, GRAPHICS_PRAMTYPE_DATA);



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


	meshParticleBufferData.counterWrapper.CopyBuffer(
		copyBuffer.GetBuffer().Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_DEST
	);


	//ワールド行列
	computeUpdateMeshParticle.SetBuffer(RENDER_PTR->GetStackBuffer(), GRAPHICS_PRAMTYPE_DATA5);
	//色
	//computeUpdateMeshParticle.SetBuffer(GPUParticleRender::Instance()->GetStackColorBuffer(), GRAPHICS_PRAMTYPE_DATA4);
	//Transformを除いたワールド行列

	float lScale = 1.0f;
	scaleRotMat = KazMath::CaluScaleMatrix({ lScale,lScale,lScale }) * KazMath::CaluRotaMatrix({ 0.0f,0.0f,0.0f });

	MESH_PARTICLE_GENERATE_NUM = 0;
}

void InstanceMeshParticle::Init()
{
	motherMatrixBuffer.CreateBuffer(
		KazBufferHelper::BufferResourceData(
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			CD3DX12_RESOURCE_DESC::Buffer(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(motherMatArray.size(), sizeof(DirectX::XMMATRIX))),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			"RAMmatData")
	);

	colorBuffer.CreateBuffer(
		KazBufferHelper::BufferResourceData(
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			CD3DX12_RESOURCE_DESC::Buffer(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(motherMatArray.size(), sizeof(float))),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			"RAMColorData")
	);

	{
		KazBufferHelper::BufferResourceData lData(
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			CD3DX12_RESOURCE_DESC::Buffer(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(motherMatArray.size(), sizeof(DirectX::XMMATRIX))),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			nullptr,
			"VRAMmatData"
		);
		lData.resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		//親行列
		particleMotherMatrixHandle = computeUpdateMeshParticle.CreateBuffer(
			lData,
			GRAPHICS_RANGE_TYPE_UAV_DESC,
			GRAPHICS_PRAMTYPE_DATA2,
			sizeof(DirectX::XMMATRIX),
			static_cast<UINT>(motherMatArray.size()),
			false
		);
	}

	{
		KazBufferHelper::BufferResourceData lData(
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			CD3DX12_RESOURCE_DESC::Buffer(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(motherMatArray.size(), sizeof(float))),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			nullptr,
			"VRAMColorData"
		);
		lData.resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		colorMotherMatrixHandle = computeUpdateMeshParticle.CreateBuffer(
			lData,
			GRAPHICS_RANGE_TYPE_UAV_DESC,
			GRAPHICS_PRAMTYPE_DATA4,
			sizeof(float),
			static_cast<UINT>(motherMatArray.size()),
			false
		);
	}


	//ScaleRotaMat
	{
		KazBufferHelper::BufferResourceData lData(
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			CD3DX12_RESOURCE_DESC::Buffer(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(scaleRotaMatArray.size(), sizeof(DirectX::XMMATRIX))),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			nullptr,
			"VRAMScaleRotaBillData"
		);
		lData.resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		//スケール
		scaleRotateBillboardMatHandle = computeUpdateMeshParticle.CreateBuffer(
			lData,
			GRAPHICS_RANGE_TYPE_UAV_DESC,
			GRAPHICS_PRAMTYPE_DATA3,
			sizeof(DirectX::XMMATRIX),
			static_cast<UINT>(scaleRotaMatArray.size()),
			false
		);
	}

	scaleRotaBuffer.CreateBuffer(
		KazBufferHelper::BufferResourceData(
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			CD3DX12_RESOURCE_DESC::Buffer(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(scaleRotaMatArray.size(), sizeof(DirectX::XMMATRIX))),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			"RAMScaleRotaBillData")
	);
}

void InstanceMeshParticle::AddMeshData(const InitMeshParticleData &DATA)
{
#pragma region 初期化用のバッファ生成

	commonColorBufferData.push_back(
		computeInitMeshParticle.CreateAndGetBuffer(
			COMMON_BUFFER_SIZE,
			GRAPHICS_RANGE_TYPE_CBV_VIEW,
			GRAPHICS_PRAMTYPE_DATA,
			1,
			false
		));

	commonBufferData.push_back(
		computeInitMeshParticle.CreateAndGetBuffer(
			sizeof(CommonData),
			GRAPHICS_RANGE_TYPE_CBV_VIEW,
			GRAPHICS_PRAMTYPE_DATA,
			1,
			false
		));



	motherMatArray.emplace_back(MotherData(DATA.motherMat, DATA.alpha));



	//何の情報を読み込むかでパイプラインの種類を変える
	computeInitMeshParticle.DeleteAllData();
	setCountNum = 0;

	IsSetBuffer(DATA.vertData);
	IsSetBuffer(DATA.uvData);


	ComputePipeLineNames lPipelineName = PIPELINE_COMPUTE_NAME_NONE;
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
		lCommonHandle = computeInitMeshParticle.SetBuffer(commonColorBufferData[commonColorBufferData.size() - 1], GRAPHICS_PRAMTYPE_DATA3);

		lCommonAndColorData.meshData = DATA.triagnleData;
		lCommonAndColorData.color = DATA.color.ConvertXMFLOAT4();
		lCommonAndColorData.id = static_cast<UINT>(MESH_PARTICLE_GENERATE_NUM);
		computeInitMeshParticle.TransData(lCommonHandle, &lCommonAndColorData, sizeof(CommonWithColorData));

		lPipelineName = PIPELINE_COMPUTE_NAME_INIT_POS_MESHPARTICLE;
		break;
	case 2:
		lCommonHandle = computeInitMeshParticle.SetBuffer(commonBufferData[commonBufferData.size() - 1], GRAPHICS_PRAMTYPE_DATA4);

		lCommonData.meshData = DATA.triagnleData;
		lCommonData.id = static_cast<UINT>(MESH_PARTICLE_GENERATE_NUM);
		computeInitMeshParticle.TransData(lCommonHandle, &lCommonData, sizeof(CommonData));

		lPipelineName = PIPELINE_COMPUTE_NAME_INIT_POSUV_MESHPARTICLE;
		break;
	default:
		break;
	}

	meshParticleOutputHandle = computeInitMeshParticle.SetBuffer(meshParticleBufferData, static_cast<GraphicsRootParamType>(GRAPHICS_PRAMTYPE_DATA + setCountNum));

	//テクスチャのセット
	if (DATA.textureHandle != -1)
	{
		ResouceBufferHelper::BufferData lData;
		lData.CreateViewHandle(DATA.textureHandle);
		lData.rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
		lData.rootParamType = GRAPHICS_PRAMTYPE_TEX;
		computeInitMeshParticle.SetBuffer(lData, GRAPHICS_PRAMTYPE_TEX);
	}

	computeInitMeshParticle.StackToCommandListAndCallDispatch(lPipelineName, { 1000,1,1 });

	++MESH_PARTICLE_GENERATE_NUM;
#pragma endregion


	scaleRotaMatArray.emplace_back(ScaleRotaBillData(KazMath::CaluScaleMatrix(DATA.particleScale), DATA.billboardFlag));
}

void InstanceMeshParticle::Compute()
{
	std::vector<DirectX::XMMATRIX>lMatArray(motherMatArray.size());
	std::vector<float>lColorArray(motherMatArray.size());
	std::vector<DirectX::XMMATRIX>lScaleMatArray(scaleRotaMatArray.size());
	for (int i = 0; i < lMatArray.size(); ++i)
	{
		lMatArray[i] = *motherMatArray[i].motherMat;
		lColorArray[i] = *motherMatArray[i].alpha;

		if (scaleRotaMatArray[i].billboardFlag)
		{
			lScaleMatArray[i] = scaleRotaMatArray[i].scaleRotaMata * CameraMgr::Instance()->GetMatBillBoard();
		}
		else
		{
			lScaleMatArray[i] = scaleRotaMatArray[i].scaleRotaMata;
		}
	}

	motherMatrixBuffer.TransData(lMatArray.data(), sizeof(DirectX::XMMATRIX) * static_cast<int>(lMatArray.size()));
	colorBuffer.TransData(lColorArray.data(), sizeof(float) * static_cast<int>(lMatArray.size()));
	scaleRotaBuffer.TransData(lScaleMatArray.data(), sizeof(DirectX::XMMATRIX) * static_cast<int>(scaleRotaMatArray.size()));

	computeUpdateMeshParticle.GetBufferData(particleMotherMatrixHandle).bufferWrapper.CopyBuffer(
		motherMatrixBuffer.GetBuffer().Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_DEST
	);

	computeUpdateMeshParticle.GetBufferData(colorMotherMatrixHandle).bufferWrapper.CopyBuffer(
		colorBuffer.GetBuffer().Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_DEST
	);

	computeUpdateMeshParticle.GetBufferData(scaleRotateBillboardMatHandle).bufferWrapper.CopyBuffer(
		scaleRotaBuffer.GetBuffer().Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_DEST
	);

	computeUpdateMeshParticle.StackToCommandListAndCallDispatch(PIPELINE_COMPUTE_NAME_UPDATE_MESHPARTICLE, { 1000,1,1 });
}