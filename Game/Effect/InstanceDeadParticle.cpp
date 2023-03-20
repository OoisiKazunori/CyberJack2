#include "InstanceDeadParticle.h"

int InstanceDeadParticle::meshIdNum = 0;

InstanceDeadParticle::InstanceDeadParticle(const GPUParticleRender *RENDER_PTR)
{
	appendParticleBufferHandle = initComputeHelper.CreateBuffer(
		KazBufferHelper::SetOnlyReadStructuredBuffer(sizeof(DeadparticleData) * PARTICLE_MAX_NUM),
		GRAPHICS_RANGE_TYPE_UAV_DESC,
		GRAPHICS_PRAMTYPE_DATA2,
		sizeof(DeadparticleData),
		PARTICLE_MAX_NUM,
		true
	);

	UINT lNum = 0;


	copyBuffer.CreateBuffer(KazBufferHelper::BufferResourceData
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		"CopyCounterBuffer"
	));
	copyBuffer.TransData(&lNum, sizeof(UINT));

	initComputeHelper.GetBufferData(appendParticleBufferHandle).counterWrapper.CopyBuffer(
		copyBuffer.GetBuffer().Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_DEST
	);



	meshIdNum = 0;


	updateComputeHelper.SetBuffer(initComputeHelper.GetBufferData(appendParticleBufferHandle), GRAPHICS_PRAMTYPE_DATA);
	updateComputeHelper.SetBuffer(RENDER_PTR->GetStackBuffer(), GRAPHICS_PRAMTYPE_DATA3);

	scaleRotateBillboardMatHandle = updateComputeHelper.CreateBuffer(
		sizeof(DirectX::XMMATRIX),
		GRAPHICS_RANGE_TYPE_CBV_VIEW,
		GRAPHICS_PRAMTYPE_DATA4,
		1,
		false
	);


	const UINT MAX = 100;
	motherMatBuffer.CreateBuffer(
		KazBufferHelper::BufferResourceData(
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			CD3DX12_RESOURCE_DESC::Buffer(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(MAX, sizeof(MotherBufferData))),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			"RAMmatData")
	);

	motherMatHandle = updateComputeHelper.CreateBuffer(
		KazBufferHelper::SetOnlyReadStructuredBuffer(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(MAX, sizeof(MotherBufferData))),
		GRAPHICS_RANGE_TYPE_UAV_DESC,
		GRAPHICS_PRAMTYPE_DATA2,
		sizeof(MotherBufferData),
		MAX,
		false
	);
}

void InstanceDeadParticle::Init()
{
}

void InstanceDeadParticle::AddData(const InitDeadParticleData &INIT_DATA)
{
	//死亡時のパーティクル情報を溜める
	idBufferData.emplace_back(
		initComputeHelper.CreateAndGetBuffer(
			sizeof(CommonData),
			GRAPHICS_RANGE_TYPE_CBV_VIEW,
			GRAPHICS_PRAMTYPE_DATA3,
			1,
			false
		));

	meshParticleData.emplace_back
	(
		MeshParticle(INIT_DATA.meshParticleData, meshIdNum)
	);


	RESOURCE_HANDLE lHandle = initComputeHelper.SetBuffer(idBufferData[idBufferData.size() - 1], GRAPHICS_PRAMTYPE_DATA3);
	CommonData lData;
	lData.id = meshIdNum;
	lData.vertNum = INIT_DATA.meshParticleData.triagnleData.x;
	initComputeHelper.TransData(lHandle, &lData, sizeof(CommonData));


	initComputeHelper.SetBuffer(meshParticleData[meshParticleData.size() - 1].GetBuffer(), GRAPHICS_PRAMTYPE_DATA);
	initComputeHelper.StackToCommandListAndCallDispatch(PIPELINE_COMPUTE_NAME_DEAD_PARTICLE_INIT, { 100,1,1 });



	matArray.emplace_back(MotherData(INIT_DATA.motherMat, INIT_DATA.startFlag));
	++meshIdNum;
}

void InstanceDeadParticle::Compute()
{
	DirectX::XMMATRIX lScaleRotaMat = KazMath::CaluScaleMatrix({ 1.0f, 1.0f, 1.0f }) * CameraMgr::Instance()->GetMatBillBoard();
	updateComputeHelper.TransData(scaleRotateBillboardMatHandle, &lScaleRotaMat, sizeof(DirectX::XMMATRIX));


	std::vector<MotherBufferData>lMatArray(matArray.size());
	for (int i = 0; i < matArray.size(); ++i)
	{
		lMatArray[i].motherMat = *matArray[i].motherMat;
		lMatArray[i].startFlag = static_cast<UINT>(*matArray[i].startFlag);
	}
	motherMatBuffer.TransData(lMatArray.data(), sizeof(MotherBufferData) * static_cast<int>(lMatArray.size()));

	updateComputeHelper.GetBufferData(motherMatHandle).bufferWrapper.CopyBuffer(
		motherMatBuffer.GetBuffer().Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_DEST
	);


	//updateComputeHelper.StackToCommandListAndCallDispatch(PIPELINE_COMPUTE_NAME_DEAD_PARTICLE_UPDATE, { 100,1,1 });
}
