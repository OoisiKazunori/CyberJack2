#include "MeshParticleRender.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"
#include"../KazLibrary/Buffer/ShaderRandomTable.h"

int MeshParticleRender::MESH_PARTICLE_GENERATE_NUM = 0;

MeshParticleRender::MeshParticleRender(const InitMeshParticleData& DATA) :
	setCountNum(0), isInitFlag(false)
{
	DescriptorHeapMgr::Instance()->SetDescriptorHeap();

	struct OutputData
	{
		DirectX::XMMATRIX mat;
		DirectX::XMFLOAT4 color;
	};

	m_particleRender = KazBufferHelper::SetGPUBufferData(sizeof(OutputData) * PARTICLE_MAX_NUM, "Particle-world-Output");
	m_particleViewProjRender.rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
	m_particleViewProjRender.rootParamType = GRAPHICS_PRAMTYPE_DATA;
	m_particleRender.structureSize = sizeof(OutputData);
	m_particleRender.elementNum = PARTICLE_MAX_NUM;
	m_particleRender.GenerateCounterBuffer();
	m_particleRender.CreateUAVView();

	m_particleViewProjRender = KazBufferHelper::SetGPUBufferData(sizeof(OutputData) * PARTICLE_MAX_NUM, "Particle-worldViewProj-Output");
	m_particleViewProjRender.rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
	m_particleViewProjRender.rootParamType = GRAPHICS_PRAMTYPE_DATA2;
	m_particleViewProjRender.structureSize = sizeof(OutputData);
	m_particleViewProjRender.elementNum = PARTICLE_MAX_NUM;
	m_particleViewProjRender.GenerateCounterBuffer();
	m_particleViewProjRender.CreateUAVView();


	m_outputMatrixBuffer = m_particleRender;

	m_meshParticleVertexBuffer = DATA.modelVertexBuffer;
	m_meshParticleIndexBuffer = DATA.modelIndexBuffer;
	//メッシュパーティクルのExcuteIndirect生成
	{
		m_executeIndirect = DrawFuncData::SetExecuteIndirect(
			DrawFuncData::GetBasicInstanceShader(),
			m_particleRender.bufferWrapper->GetBuffer()->GetGPUVirtualAddress(),
			PARTICLE_MAX_NUM
		);
	}

	m_executeIndirect.extraBufferArray.emplace_back(m_particleRender);
	m_executeIndirect.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_executeIndirect.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
	m_viewBuffer = KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX));


	std::vector<KazBufferHelper::BufferData>buffer;
	buffer.emplace_back(m_particleRender);
	buffer.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
	buffer.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

	buffer.emplace_back(m_particleViewProjRender);
	buffer.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
	buffer.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;

	buffer.emplace_back(m_viewBuffer);
	buffer.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
	buffer.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

	buffer.emplace_back(m_meshParticleVertexBuffer);
	buffer.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	buffer.back().rootParamType = GRAPHICS_PRAMTYPE_DATA3;

	buffer.emplace_back(m_meshParticleIndexBuffer);
	buffer.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	buffer.back().rootParamType = GRAPHICS_PRAMTYPE_DATA4;
	m_computeCuring.Generate(
		ShaderOptionData("Resource/ShaderFiles/ShaderFile/ConvertWorldToProj.hlsl", "CSmain", "cs_6_4", SHADER_TYPE_COMPUTE),
		buffer
	);






	//メッシュパーティクルの初期化処理の出力情報
	meshParticleBufferData = KazBufferHelper::SetGPUBufferData(sizeof(InitOutputData) * PARTICLE_MAX_NUM, "Stack-Meshparticle");
	meshParticleBufferData.rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
	meshParticleBufferData.rootParamType = GRAPHICS_PRAMTYPE_DATA;
	meshParticleBufferData.structureSize = sizeof(InitOutputData);
	meshParticleBufferData.elementNum = PARTICLE_MAX_NUM;
	meshParticleBufferData.bufferWrapper->ChangeBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
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
	//Transformを除いたワールド行列

	float lScale = 1.0f;
	scaleRotMat = KazMath::CaluScaleMatrix({ lScale,lScale,lScale }) * KazMath::CaluRotaMatrix({ 0.0f,0.0f,0.0f });

	MESH_PARTICLE_GENERATE_NUM = 0;

	{
		RootSignatureDataTest rootsignature;
		rootsignature.rangeArray.emplace_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_UAV_DESC, GRAPHICS_PRAMTYPE_DATA));
		rootsignature.rangeArray.emplace_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA2));
		rootsignature.rangeArray.emplace_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA3));
		rootsignature.rangeArray.emplace_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA4));
		rootsignature.rangeArray.emplace_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_UAV_DESC, GRAPHICS_PRAMTYPE_DATA5));
		rootsignature.rangeArray.emplace_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA6));
		rootsignature.rangeArray.emplace_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA7));
		rootsignature.rangeArray.emplace_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA));
		rootsignature.rangeArray.emplace_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA8));
		computeUpdateMeshParticle.Generate(ShaderOptionData("Resource/ShaderFiles/ComputeShader/UpdateMeshParticle.hlsl", "CSmain", "cs_6_4", SHADER_TYPE_COMPUTE), rootsignature);
	}
	cameraMatBuffer = KazBufferHelper::SetConstBufferData(sizeof(CameraMatData));

	{
		RootSignatureDataTest rootsignature;
		rootsignature.rangeArray.emplace_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA));
		rootsignature.rangeArray.emplace_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_UAV_DESC, GRAPHICS_PRAMTYPE_DATA2));
		rootsignature.rangeArray.emplace_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA));
		m_computeStackVertex.Generate(ShaderOptionData("Resource/ShaderFiles/ShaderFile/InputVertex.hlsl", "CSmain", "cs_6_4", SHADER_TYPE_COMPUTE), rootsignature);
		m_computeStackVertex.m_extraBufferArray.resize(3);
	}
	struct Input
	{
		DirectX::XMFLOAT3 pos;
		UINT id;
	};
	const int VERT_MAX_NUM = 50000;
	m_inputVertexBuffer = KazBufferHelper::SetGPUBufferData(sizeof(Input) * VERT_MAX_NUM, "VertexBufferArray");
	m_inputVertexBuffer.structureSize = sizeof(Input);
	m_inputVertexBuffer.elementNum = VERT_MAX_NUM;
	m_inputVertexBuffer.bufferWrapper->ChangeBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	m_inputVertexBuffer.GenerateCounterBuffer();
	m_inputVertexBuffer.CreateUAVView();
	m_inputVertexBuffer.counterWrapper->CopyBuffer(copyBuffer.GetBuffer());

	m_meshParticleIndexBuffer.bufferWrapper->ChangeBarrier(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	AddMeshData(DATA);
	m_meshParticleIndexBuffer.bufferWrapper->ChangeBarrier(D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);

	Init();
}

void MeshParticleRender::Init()
{
	motherMatrixBuffer.CreateBuffer(
		KazBufferHelper::BufferResourceData(
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			CD3DX12_RESOURCE_DESC::Buffer(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(MOTHER_MAT_MAX, sizeof(MotherMatData))),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			"RAMmatData")
	);

	curlNoizeUploadBuffer = KazBufferHelper::SetUploadBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(MOTHER_MAT_MAX, sizeof(UINT)));

	colorBuffer.CreateBuffer(
		KazBufferHelper::BufferResourceData(
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			CD3DX12_RESOURCE_DESC::Buffer(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(MOTHER_MAT_MAX, sizeof(float))),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			"RAMColorData")
	);

	{
		//親行列
		particleMotherMatrixHandle = KazBufferHelper::SetGPUBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(MOTHER_MAT_MAX, sizeof(MotherMatData)), "VRAMmatData");
		particleMotherMatrixHandle.rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		particleMotherMatrixHandle.rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		particleMotherMatrixHandle.elementNum = static_cast<UINT>(MOTHER_MAT_MAX);
		particleMotherMatrixHandle.structureSize = sizeof(MotherMatData);
	}

	{
		//色
		colorMotherMatrixHandle = KazBufferHelper::SetGPUBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(MOTHER_MAT_MAX, sizeof(float)), "VRAMColorData");
		colorMotherMatrixHandle.rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		colorMotherMatrixHandle.rootParamType = GRAPHICS_PRAMTYPE_DATA4;
		colorMotherMatrixHandle.elementNum = static_cast<UINT>(MOTHER_MAT_MAX);
		colorMotherMatrixHandle.structureSize = sizeof(float);
	}

	curlNoizeVRAMBuffer = KazBufferHelper::SetGPUBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(MOTHER_MAT_MAX, sizeof(UINT)), "CurlNoize-VRAM");
	curlNoizeVRAMBuffer.rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	curlNoizeVRAMBuffer.rootParamType = GRAPHICS_PRAMTYPE_DATA6;
	curlNoizeVRAMBuffer.elementNum = static_cast<UINT>(MOTHER_MAT_MAX);
	curlNoizeVRAMBuffer.structureSize = sizeof(UINT);

	//ScaleRotaMat
	{
		scaleRotateBillboardMatHandle = KazBufferHelper::SetGPUBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(MOTHER_MAT_MAX, sizeof(DirectX::XMMATRIX)), "VRAMScaleRotaBillData");
		scaleRotateBillboardMatHandle.rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		scaleRotateBillboardMatHandle.rootParamType = GRAPHICS_PRAMTYPE_DATA3;
		scaleRotateBillboardMatHandle.elementNum = static_cast<UINT>(MOTHER_MAT_MAX);
		scaleRotateBillboardMatHandle.structureSize = sizeof(DirectX::XMMATRIX);
	}

	scaleRotaBuffer.CreateBuffer(
		KazBufferHelper::BufferResourceData(
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			CD3DX12_RESOURCE_DESC::Buffer(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(MOTHER_MAT_MAX, sizeof(DirectX::XMMATRIX))),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			"RAMScaleRotaBillData")
	);



	computeUpdateMeshParticle.m_extraBufferArray.emplace_back(meshParticleBufferData);
	computeUpdateMeshParticle.m_extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
	computeUpdateMeshParticle.m_extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

	computeUpdateMeshParticle.m_extraBufferArray.emplace_back(particleMotherMatrixHandle);
	computeUpdateMeshParticle.m_extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	computeUpdateMeshParticle.m_extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;

	computeUpdateMeshParticle.m_extraBufferArray.emplace_back(scaleRotateBillboardMatHandle);
	computeUpdateMeshParticle.m_extraBufferArray[2].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	computeUpdateMeshParticle.m_extraBufferArray[2].rootParamType = GRAPHICS_PRAMTYPE_DATA3;

	computeUpdateMeshParticle.m_extraBufferArray.emplace_back(colorMotherMatrixHandle);
	computeUpdateMeshParticle.m_extraBufferArray[3].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	computeUpdateMeshParticle.m_extraBufferArray[3].rootParamType = GRAPHICS_PRAMTYPE_DATA4;

	computeUpdateMeshParticle.m_extraBufferArray.emplace_back(m_outputMatrixBuffer);
	computeUpdateMeshParticle.m_extraBufferArray[4].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
	computeUpdateMeshParticle.m_extraBufferArray[4].rootParamType = GRAPHICS_PRAMTYPE_DATA5;

	computeUpdateMeshParticle.m_extraBufferArray.emplace_back(cameraMatBuffer);
	computeUpdateMeshParticle.m_extraBufferArray[5].rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
	computeUpdateMeshParticle.m_extraBufferArray[5].rootParamType = GRAPHICS_PRAMTYPE_DATA;

	computeUpdateMeshParticle.m_extraBufferArray.emplace_back(ShaderRandomTable::Instance()->GetCurlBuffer(GRAPHICS_PRAMTYPE_DATA6));

	computeUpdateMeshParticle.m_extraBufferArray.emplace_back(curlNoizeVRAMBuffer);
	computeUpdateMeshParticle.m_extraBufferArray[7].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	computeUpdateMeshParticle.m_extraBufferArray[7].rootParamType = GRAPHICS_PRAMTYPE_DATA7;

	computeUpdateMeshParticle.m_extraBufferArray.emplace_back(m_meshParticleVertexBuffer);
	computeUpdateMeshParticle.m_extraBufferArray[8].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	computeUpdateMeshParticle.m_extraBufferArray[8].rootParamType = GRAPHICS_PRAMTYPE_DATA8;


	m_initParticleBuffer = KazBufferHelper::SetGPUBufferData(sizeof(InitOutputData) * PARTICLE_MAX_NUM, "Init - MeshParticle");
	m_initParticleBuffer.bufferWrapper->ChangeBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	meshParticleBufferData.bufferWrapper->ChangeBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	m_initParticleBuffer.bufferWrapper->CopyBuffer(meshParticleBufferData.bufferWrapper->GetBuffer());
	meshParticleBufferData.bufferWrapper->ChangeBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

}

void MeshParticleRender::AddMeshData(const InitMeshParticleData& DATA)
{
#pragma region 初期化用のバッファ生成

	initData.emplace_back(DATA);


	commonColorBufferData.emplace_back(KazBufferHelper::SetConstBufferData(COMMON_BUFFER_SIZE));
	commonColorBufferData.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
	commonColorBufferData.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

	commonBufferData.emplace_back(KazBufferHelper::SetConstBufferData(sizeof(CommonData)));
	commonBufferData.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
	commonBufferData.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;


	motherMatArray.emplace_back(MotherData(DATA.motherMat, DATA.alpha, DATA.curlNoizeFlag));



	//何の情報を読み込むかでパイプラインの種類を変える

	std::vector<KazBufferHelper::BufferData>bufferArray;
	setCountNum = 0;

	IsSetBuffer(DATA.vertData, bufferArray);
	IsSetBuffer(DATA.uvData, bufferArray);
	IsSetBuffer(DATA.modelIndexBuffer, bufferArray);


	CommonWithColorData commonAndColorData;
	CommonData commonData;
	switch (setCountNum)
	{
	case 0:
		//メッシュパーティクルに必要な情報が何も入ってない
		assert(0);
		break;
	case 2:
		bufferArray.emplace_back(commonColorBufferData[commonColorBufferData.size() - 1]);
		bufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		bufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA3;

		commonAndColorData.meshData = DATA.triagnleData;
		commonAndColorData.color = DATA.color.ConvertXMFLOAT4();
		commonAndColorData.id = static_cast<UINT>(MESH_PARTICLE_GENERATE_NUM);
		bufferArray.back().bufferWrapper->TransData(&commonAndColorData, sizeof(CommonWithColorData));

		break;
	case 3:
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
	computeInitMeshParticle.Compute({ MOTHER_MAT_MAX,1,1 });

#pragma endregion


	scaleRotaMatArray.emplace_back(ScaleRotaBillData(KazMath::CaluScaleMatrix(DATA.particleScale), DATA.billboardFlag));
}

void MeshParticleRender::Compute(DrawingByRasterize& arg_rasterize)
{
	m_particleRender.counterWrapper->CopyBuffer(copyBuffer.GetBuffer());
	m_particleViewProjRender.counterWrapper->CopyBuffer(copyBuffer.GetBuffer());

	std::vector<MotherMatData>lMatArray(MOTHER_MAT_MAX);
	std::vector<float>lColorArray(MOTHER_MAT_MAX);
	std::vector<DirectX::XMMATRIX>lScaleMatArray(MOTHER_MAT_MAX);
	std::vector<int>curlNoizeArray(MOTHER_MAT_MAX);
	for (int i = 0; i < lMatArray.size(); ++i)
	{
		if (motherMatArray.size() <= i)
		{
			lMatArray[i].motherMat = DirectX::XMMatrixIdentity();
			lColorArray[i] = 0.0f;
			curlNoizeArray[i] = 0;
			lScaleMatArray[i] = DirectX::XMMatrixIdentity();
			continue;
		}

		//行列
		if (motherMatArray[i].motherMat)
		{
			lMatArray[i].motherMat = *motherMatArray[i].motherMat;
		}
		else
		{
			lMatArray[i].motherMat = DirectX::XMMatrixIdentity();
		}
		//色
		if (motherMatArray[i].alpha)
		{
			lColorArray[i] = *motherMatArray[i].alpha;
		}
		else
		{
			lColorArray[i] = 0.0f;
		}

		if (scaleRotaMatArray[i].billboardFlag)
		{
			lScaleMatArray[i] = scaleRotaMatArray[i].scaleRotaMata * CameraMgr::Instance()->GetMatBillBoard();
		}
		else
		{
			lScaleMatArray[i] = scaleRotaMatArray[i].scaleRotaMata;
		}
		//カールノイズ
		if (motherMatArray[i].curlNozieFlag)
		{
			curlNoizeArray[i] = static_cast<int>(*motherMatArray[i].curlNozieFlag);
		}
		else
		{
			curlNoizeArray[i] = 0;
		}
	}

	motherMatrixBuffer.TransData(lMatArray.data(), sizeof(MotherMatData) * static_cast<int>(lMatArray.size()));
	colorBuffer.TransData(lColorArray.data(), sizeof(float) * static_cast<int>(lMatArray.size()));
	scaleRotaBuffer.TransData(lScaleMatArray.data(), sizeof(DirectX::XMMATRIX) * static_cast<int>(MOTHER_MAT_MAX));
	curlNoizeUploadBuffer.bufferWrapper->TransData(curlNoizeArray.data(), sizeof(UINT) * static_cast<int>(MOTHER_MAT_MAX));


	particleMotherMatrixHandle.bufferWrapper->CopyBuffer(
		motherMatrixBuffer.GetBuffer().Get());

	colorMotherMatrixHandle.bufferWrapper->CopyBuffer(
		colorBuffer.GetBuffer().Get());

	scaleRotateBillboardMatHandle.bufferWrapper->CopyBuffer(
		scaleRotaBuffer.GetBuffer().Get());

	curlNoizeVRAMBuffer.bufferWrapper->CopyBuffer(
		curlNoizeUploadBuffer.bufferWrapper->GetBuffer().Get());

	CameraMatData camera;
	camera.viewProjMat = CameraMgr::Instance()->GetViewMatrix() * CameraMgr::Instance()->GetPerspectiveMatProjection();
	camera.billboard = CameraMgr::Instance()->GetMatBillBoard();
	camera.indexNum = initData.back().triagnleData.w;
	cameraMatBuffer.bufferWrapper->TransData(&camera, sizeof(CameraMatData));

	m_meshParticleVertexBuffer.bufferWrapper->ChangeBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	computeUpdateMeshParticle.Compute({ DISPATCH_NUM,1,1 });
	m_meshParticleVertexBuffer.bufferWrapper->ChangeBarrier(D3D12_RESOURCE_STATE_COMMON);

	//描画処理---------------------------------------
	arg_rasterize.ObjectRender(m_executeIndirect);
	//描画処理---------------------------------------

}

void MeshParticleRender::InitCompute()
{
	meshParticleBufferData.counterWrapper->CopyBuffer(copyBuffer.GetBuffer());
	computeInitMeshParticle.Compute({ DISPATCH_NUM,1,1 });
}

void MeshParticleRender::InitParticle()
{
	meshParticleBufferData.bufferWrapper->CopyBuffer(m_initParticleBuffer.bufferWrapper->GetBuffer());
}
