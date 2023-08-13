#include "InstanceMeshParticle.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"

int InstanceMeshParticle::MESH_PARTICLE_GENERATE_NUM = 0;

InstanceMeshParticle::InstanceMeshParticle(const KazBufferHelper::BufferData& arg_outputMat, const KazBufferHelper::BufferData& arg_colorBuffer) :
	setCountNum(0), m_outputMatrixBuffer(arg_outputMat), m_outputColorBuffer(arg_colorBuffer), isInitFlag(false)
{
	//���b�V���p�[�e�B�N���̏����������̏o�͏��
	meshParticleBufferData = KazBufferHelper::SetGPUBufferData(sizeof(InitOutputData) * PARTICLE_MAX_NUM);
	meshParticleBufferData.rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
	meshParticleBufferData.rootParamType = GRAPHICS_PRAMTYPE_DATA;
	meshParticleBufferData.structureSize = sizeof(InitOutputData);
	meshParticleBufferData.elementNum = PARTICLE_MAX_NUM;
	meshParticleBufferData.bufferWrapper->ChangeBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	meshParticleBufferData.GenerateCounterBuffer();
	meshParticleBufferData.CreateUAVView();



	////�p�[�e�B�N���f�[�^
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


	//���[���h�s��
	m_outputMatrixBuffer.rootParamType = GRAPHICS_PRAMTYPE_DATA5;
	//�F
	m_outputColorBuffer.rootParamType = GRAPHICS_PRAMTYPE_DATA4;
	//Transform�����������[���h�s��

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
		//�e�s��
		particleMotherMatrixHandle = KazBufferHelper::SetGPUBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(motherMatArray.size(), sizeof(DirectX::XMMATRIX)), "VRAMmatData");
		particleMotherMatrixHandle.rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		particleMotherMatrixHandle.rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		particleMotherMatrixHandle.elementNum = static_cast<UINT>(motherMatArray.size());
		particleMotherMatrixHandle.structureSize = sizeof(DirectX::XMMATRIX);
	}

	{
		//�F
		colorMotherMatrixHandle = KazBufferHelper::SetGPUBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(motherMatArray.size(), sizeof(float)), "VRAMColorData");
		colorMotherMatrixHandle.rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		colorMotherMatrixHandle.rootParamType = GRAPHICS_PRAMTYPE_DATA4;
		colorMotherMatrixHandle.elementNum = static_cast<UINT>(motherMatArray.size());
		colorMotherMatrixHandle.structureSize = sizeof(float);
	}


	//ScaleRotaMat
	{
		scaleRotateBillboardMatHandle = KazBufferHelper::SetGPUBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(scaleRotaMatArray.size(), sizeof(DirectX::XMMATRIX)), "VRAMScaleRotaBillData");
		scaleRotateBillboardMatHandle.rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
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

}

void InstanceMeshParticle::AddMeshData(const InitMeshParticleData& DATA)
{
#pragma region �������p�̃o�b�t�@����


	commonColorBufferData.emplace_back(KazBufferHelper::SetConstBufferData(COMMON_BUFFER_SIZE));
	commonColorBufferData.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
	commonColorBufferData.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

	commonBufferData.emplace_back(KazBufferHelper::SetConstBufferData(sizeof(CommonData)));
	commonBufferData.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
	commonBufferData.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;


	motherMatArray.emplace_back(MotherData(DATA.motherMat, DATA.alpha));



	//���̏���ǂݍ��ނ��Ńp�C�v���C���̎�ނ�ς���

	std::vector<KazBufferHelper::BufferData>bufferArray;
	setCountNum = 0;

	IsSetBuffer(DATA.vertData, bufferArray);
	IsSetBuffer(DATA.uvData, bufferArray);


	CommonWithColorData commonAndColorData;
	CommonData commonData;
	switch (setCountNum)
	{
	case 0:
		//���b�V���p�[�e�B�N���ɕK�v�ȏ�񂪉��������ĂȂ�
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
		bufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA4;

		commonData.meshData = DATA.triagnleData;
		commonData.id = static_cast<UINT>(MESH_PARTICLE_GENERATE_NUM);
		bufferArray.back().bufferWrapper->TransData(&commonData, sizeof(CommonData));
		break;
	default:
		break;
	}

	bufferArray.emplace_back(meshParticleBufferData);
	bufferArray.back().rootParamType = static_cast<GraphicsRootParamType>(GRAPHICS_PRAMTYPE_DATA + setCountNum);

	//�e�N�X�`���̃Z�b�g
	if (DATA.textureHandle != -1)
	{
		KazBufferHelper::BufferData lData;
		lData.bufferWrapper->CreateViewHandle(DATA.textureHandle);
		lData.rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
		lData.rootParamType = GRAPHICS_PRAMTYPE_TEX;
		bufferArray.emplace_back(lData);
	}


	computeInitMeshParticle.m_extraBufferArray = bufferArray;
	if (!isInitFlag)
	{
		computeInitMeshParticle.Generate(ShaderOptionData("Resource/ShaderFiles/ComputeShader/MeshParticleComputeShader.hlsl", "CSmain", "cs_6_4", SHADER_TYPE_COMPUTE), bufferArray);
		isInitFlag = false;
	}
	computeInitMeshParticle.Compute({ 1,1,1 });

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

	computeUpdateMeshParticle.Compute({ 1000,1,1 });
}