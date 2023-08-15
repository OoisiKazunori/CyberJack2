#include "InstanceMeshParticle.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"
#include"../KazLibrary/Buffer/ShaderRandomTable.h"

int InstanceMeshParticle::MESH_PARTICLE_GENERATE_NUM = 0;

InstanceMeshParticle::InstanceMeshParticle(const KazBufferHelper::BufferData& arg_outputMat, const KazBufferHelper::BufferData& arg_colorBuffer) :
	setCountNum(0), m_outputMatrixBuffer(arg_outputMat), m_outputColorBuffer(arg_colorBuffer), isInitFlag(false)
{
	//メッシュパーティクルの初期化処理の出力情報
	meshParticleBufferData = KazBufferHelper::SetGPUBufferData(sizeof(InitOutputData) * PARTICLE_MAX_NUM);
	meshParticleBufferData.rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
	meshParticleBufferData.rootParamType = GRAPHICS_PRAMTYPE_DATA;
	meshParticleBufferData.structureSize = sizeof(InitOutputData);
	meshParticleBufferData.elementNum = PARTICLE_MAX_NUM;
	meshParticleBufferData.bufferWrapper->ChangeBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	meshParticleBufferData.GenerateCounterBuffer();
	meshParticleBufferData.CreateUAVView();



	////パーティクルデータ
	//computeUpdateMeshParticle.SetBuffer(meshParticleBufferData, GRAPHICS_PRAMTYPE_DATA);


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


	//ワールド行列
	m_outputMatrixBuffer.rootParamType = GRAPHICS_PRAMTYPE_DATA5;
	//色
	m_outputColorBuffer.rootParamType = GRAPHICS_PRAMTYPE_DATA4;
	//Transformを除いたワールド行列

	float lScale = 1.0f;
	scaleRotMat = KazMath::CaluScaleMatrix({ lScale,lScale,lScale }) * KazMath::CaluRotaMatrix({ 0.0f,0.0f,0.0f });

	MESH_PARTICLE_GENERATE_NUM = 0;

	RootSignatureDataTest rootsignature;
	rootsignature.rangeArray.emplace_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_UAV_DESC, GRAPHICS_PRAMTYPE_DATA));
	rootsignature.rangeArray.emplace_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA2));
	rootsignature.rangeArray.emplace_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA3));
	rootsignature.rangeArray.emplace_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA4));
	rootsignature.rangeArray.emplace_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_UAV_DESC, GRAPHICS_PRAMTYPE_DATA5));
	rootsignature.rangeArray.emplace_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA));
	rootsignature.rangeArray.emplace_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA6));
	computeUpdateMeshParticle.Generate(ShaderOptionData("Resource/ShaderFiles/ComputeShader/UpdateMeshParticle.hlsl", "CSmain", "cs_6_4", SHADER_TYPE_COMPUTE), rootsignature);

	cameraMatBuffer = KazBufferHelper::SetConstBufferData(sizeof(CameraMatData));
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
		//親行列
		particleMotherMatrixHandle = KazBufferHelper::SetGPUBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(motherMatArray.size(), sizeof(DirectX::XMMATRIX)), "VRAMmatData");
		particleMotherMatrixHandle.rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		particleMotherMatrixHandle.rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		particleMotherMatrixHandle.elementNum = static_cast<UINT>(motherMatArray.size());
		particleMotherMatrixHandle.structureSize = sizeof(DirectX::XMMATRIX);
	}

	{
		//色
		colorMotherMatrixHandle = KazBufferHelper::SetGPUBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(motherMatArray.size(), sizeof(float)), "VRAMColorData");
		colorMotherMatrixHandle.rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		colorMotherMatrixHandle.rootParamType = GRAPHICS_PRAMTYPE_DATA4;
		colorMotherMatrixHandle.elementNum = static_cast<UINT>(motherMatArray.size());
		colorMotherMatrixHandle.structureSize = sizeof(float);
	}


	//ScaleRotaMat
	{
		scaleRotateBillboardMatHandle = KazBufferHelper::SetGPUBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(scaleRotaMatArray.size(), sizeof(DirectX::XMMATRIX)), "VRAMScaleRotaBillData");
		scaleRotateBillboardMatHandle.rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		scaleRotateBillboardMatHandle.rootParamType = GRAPHICS_PRAMTYPE_DATA3;
		scaleRotateBillboardMatHandle.elementNum = static_cast<UINT>(scaleRotaMatArray.size());
		scaleRotateBillboardMatHandle.structureSize = sizeof(DirectX::XMMATRIX);
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



	computeUpdateMeshParticle.m_extraBufferArray.emplace_back(meshParticleBufferData);
	computeUpdateMeshParticle.m_extraBufferArray.emplace_back(particleMotherMatrixHandle);
	computeUpdateMeshParticle.m_extraBufferArray.emplace_back(scaleRotateBillboardMatHandle);
	computeUpdateMeshParticle.m_extraBufferArray.emplace_back(colorMotherMatrixHandle);
	computeUpdateMeshParticle.m_extraBufferArray.emplace_back(m_outputMatrixBuffer);
	computeUpdateMeshParticle.m_extraBufferArray.emplace_back(cameraMatBuffer);
	computeUpdateMeshParticle.m_extraBufferArray.emplace_back(ShaderRandomTable::Instance()->GetBuffer(GRAPHICS_PRAMTYPE_DATA6));

}

void InstanceMeshParticle::AddMeshData(const InitMeshParticleData& DATA)
{
#pragma region 初期化用のバッファ生成


	commonColorBufferData.emplace_back(KazBufferHelper::SetConstBufferData(COMMON_BUFFER_SIZE));
	commonColorBufferData.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
	commonColorBufferData.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

	commonBufferData.emplace_back(KazBufferHelper::SetConstBufferData(sizeof(CommonData)));
	commonBufferData.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
	commonBufferData.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;


	motherMatArray.emplace_back(MotherData(DATA.motherMat, DATA.alpha));



	//何の情報を読み込むかでパイプラインの種類を変える

	std::vector<KazBufferHelper::BufferData>bufferArray;
	setCountNum = 0;

	IsSetBuffer(DATA.vertData, bufferArray);
	IsSetBuffer(DATA.uvData, bufferArray);


	CommonWithColorData commonAndColorData;
	CommonData commonData;
	switch (setCountNum)
	{
	case 0:
		//メッシュパーティクルに必要な情報が何も入ってない
		assert(0);
		break;
	case 1:
		bufferArray.emplace_back(commonColorBufferData[commonColorBufferData.size() - 1]);
		bufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		bufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA3;

		commonAndColorData.meshData = DATA.triagnleData;
		commonAndColorData.color = DATA.color.ConvertXMFLOAT4();
		commonAndColorData.id = static_cast<UINT>(MESH_PARTICLE_GENERATE_NUM);
		bufferArray.back().bufferWrapper->TransData(&commonAndColorData, sizeof(CommonWithColorData));

		break;
	case 2:
		bufferArray.emplace_back(commonBufferData[commonBufferData.size() - 1]);
		bufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		bufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		commonData.meshData = DATA.triagnleData;
		commonData.id = static_cast<UINT>(MESH_PARTICLE_GENERATE_NUM);
		bufferArray.back().bufferWrapper->TransData(&commonData, sizeof(CommonData));
		break;
	default:
		break;
	}

	bufferArray.emplace_back(meshParticleBufferData);
	bufferArray.back().rootParamType = static_cast<GraphicsRootParamType>(GRAPHICS_PRAMTYPE_DATA + setCountNum);

	//テクスチャのセット
	//if (DATA.textureHandle != -1)
	{
		bufferArray.emplace_back(TextureResourceMgr::Instance()->LoadGraphBuffer(KazFilePathName::TestPath + "Test.png"));
		bufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
		bufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_TEX;
	}


	computeInitMeshParticle.m_extraBufferArray = bufferArray;
	for (int i = 0; i < bufferArray.size(); ++i)
	{
		computeInitMeshParticle.m_extraBufferArray[i].rangeType = bufferArray[i].rangeType;
		computeInitMeshParticle.m_extraBufferArray[i].rootParamType = bufferArray[i].rootParamType;
	}
	if (!isInitFlag)
	{
		computeInitMeshParticle.Generate(ShaderOptionData("Resource/ShaderFiles/ComputeShader/InitPosUvMeshParticle.hlsl", "CSmain", "cs_6_4", SHADER_TYPE_COMPUTE), bufferArray);
		isInitFlag = false;
	}
	computeInitMeshParticle.Compute({ 100,1,1 });

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

	particleMotherMatrixHandle.bufferWrapper->CopyBuffer(
		motherMatrixBuffer.GetBuffer().Get());

	colorMotherMatrixHandle.bufferWrapper->CopyBuffer(
		colorBuffer.GetBuffer().Get());

	scaleRotateBillboardMatHandle.bufferWrapper->CopyBuffer(
		scaleRotaBuffer.GetBuffer().Get());

	CameraMatData camera;
	camera.viewProjMat = CameraMgr::Instance()->GetViewMatrix() * CameraMgr::Instance()->GetPerspectiveMatProjection();
	camera.billboard = CameraMgr::Instance()->GetMatBillBoard();
	cameraMatBuffer.bufferWrapper->TransData(&camera, sizeof(CameraMatData));

	computeUpdateMeshParticle.m_extraBufferArray[0] = meshParticleBufferData;
	computeUpdateMeshParticle.m_extraBufferArray[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
	computeUpdateMeshParticle.m_extraBufferArray[0].rootParamType = GRAPHICS_PRAMTYPE_DATA;

	computeUpdateMeshParticle.m_extraBufferArray[1] = particleMotherMatrixHandle;
	computeUpdateMeshParticle.m_extraBufferArray[1].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	computeUpdateMeshParticle.m_extraBufferArray[1].rootParamType = GRAPHICS_PRAMTYPE_DATA2;

	computeUpdateMeshParticle.m_extraBufferArray[2] = scaleRotateBillboardMatHandle;
	computeUpdateMeshParticle.m_extraBufferArray[2].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	computeUpdateMeshParticle.m_extraBufferArray[2].rootParamType = GRAPHICS_PRAMTYPE_DATA3;

	computeUpdateMeshParticle.m_extraBufferArray[3] = colorMotherMatrixHandle;
	computeUpdateMeshParticle.m_extraBufferArray[3].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	computeUpdateMeshParticle.m_extraBufferArray[3].rootParamType = GRAPHICS_PRAMTYPE_DATA4;

	computeUpdateMeshParticle.m_extraBufferArray[4] = m_outputMatrixBuffer;
	computeUpdateMeshParticle.m_extraBufferArray[4].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
	computeUpdateMeshParticle.m_extraBufferArray[4].rootParamType = GRAPHICS_PRAMTYPE_DATA5;

	computeUpdateMeshParticle.m_extraBufferArray[5] = cameraMatBuffer;
	computeUpdateMeshParticle.m_extraBufferArray[5].rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
	computeUpdateMeshParticle.m_extraBufferArray[5].rootParamType = GRAPHICS_PRAMTYPE_DATA;

	computeUpdateMeshParticle.m_extraBufferArray[6] = ShaderRandomTable::Instance()->GetBuffer(GRAPHICS_PRAMTYPE_DATA6);

	computeUpdateMeshParticle.Compute({ 100,1,1 });
}

void InstanceMeshParticle::InitCompute()
{
	meshParticleBufferData.counterWrapper->CopyBuffer(copyBuffer.GetBuffer());
	computeInitMeshParticle.Compute({ 100,1,1 });
}
