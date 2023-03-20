#include"PortalIntermediateDirection.h"
#include"../KazLibrary/RenderTarget/RenderTargetStatus.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"
#include"../KazLibrary/Buffer/UavViewHandleMgr.h"
#include"../KazLibrary/Easing/easing.h"

PortalIntermediateDirection::PortalIntermediateDirection()
{
	renderTarget = std::make_unique<GameRenderTarget>(KazMath::Color(14, 12, 13, 255));


	portalRender.data.handleData = renderTarget->GetGameRenderTargetHandle();
	portalRender.data.transform.pos = { 0.0f,10.0f,30.0f };
	portalRender.data.transform.scale = { 0.3f,0.3f,0.0f };
	portalRender.data.pipelineName = PIPELINE_NAME_PORTAL;

	nextPortalRender.data.transform.pos = { 0.0f,0.0f,40.0f };
	nextPortalRender.data.transform.scale = { 0.2f,0.2f,0.0f };
	nextPortalRender.data.pipelineName = PIPELINE_NAME_PORTAL;

	buffers = std::make_unique<CreateGpuBuffer>();

	player.Init({ 0.0f,0.0f,0.0f }, false, false);


	//コマンドシグネチャ---------------------------
	std::array<D3D12_INDIRECT_ARGUMENT_DESC, 2> args{};
	args[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_UNORDERED_ACCESS_VIEW;
	args[0].UnorderedAccessView.RootParameterIndex = 0;
	args[1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

	D3D12_COMMAND_SIGNATURE_DESC desc{};
	desc.pArgumentDescs = args.data();
	desc.NumArgumentDescs = static_cast<UINT>(args.size());
	desc.ByteStride = sizeof(IndirectCommand);

	HRESULT lR =
		DirectX12Device::Instance()->dev->CreateCommandSignature(&desc, GraphicsRootSignature::Instance()->GetRootSignature(ROOTSIGNATURE_DATA_DRAW_UAV).Get(), IID_PPV_ARGS(&commandSig));
	//コマンドシグネチャ---------------------------
	if (lR != S_OK)
	{
		assert(0);
	}

	const float lSize = 1.0f;
	Vertex lVertices[] =
	{
		//x,y,z	法線	u,v
	//前
	{{-lSize, -lSize, -lSize},{}, {0.0f,1.0f}},		//左下
	{{-lSize,  lSize, -lSize},{}, {0.0f,0.0f}},		//左上
	{{ lSize, -lSize, -lSize}, {}, {1.0f,1.0f}},		//右下
	{{ lSize,  lSize, -lSize}, {}, {1.0f,0.0f}},		//右上

	//後
	{{-lSize,-lSize,  lSize}, {}, {0.0f,1.0f}},		//左下
	{{-lSize, lSize,  lSize}, {},{0.0f,0.0f}},		//左上
	{{ lSize,-lSize,  lSize},{},{1.0f,1.0f}},		//右下
	{{ lSize, lSize,  lSize},{}, {1.0f,0.0f}},		//右上

	//左
	{{-lSize,-lSize, -lSize},{}, {0.0f,1.0f}},		//左下
	{{-lSize,-lSize,  lSize}, {}, {0.0f,0.0f}},		//左上
	{{-lSize, lSize, -lSize}, {}, {1.0f,1.0f}},		//右下
	{{-lSize, lSize,  lSize}, {}, {1.0f,0.0f}},		//右上

	//右
	{{lSize,-lSize, -lSize},{}, {0.0f,1.0f}},		//左下
	{{lSize,-lSize,  lSize}, {}, {0.0f,0.0f}},		//左上
	{{lSize, lSize, -lSize}, {}, {1.0f,1.0f}},		//右下
	{{lSize, lSize,  lSize}, {}, {1.0f,0.0f}},		//右上

	//下
	{{ lSize, -lSize, lSize}, {}, {0.0f,1.0f}},		//左下
	{{ lSize, -lSize,-lSize}, {}, {0.0f,0.0f}},		//左上
	{{-lSize, -lSize, lSize}, {}, {1.0f,1.0f}},		//右下
	{{-lSize, -lSize,-lSize}, {}, {1.0f,0.0f}},	//右上

	//上
	{{ lSize, lSize, lSize}, {}, {0.0f,1.0f}},			//左下
	{{ lSize, lSize,-lSize}, {}, {0.0f,0.0f}},			//左上
	{{-lSize, lSize, lSize}, {}, {1.0f,1.0f}},			//右下
	{{-lSize, lSize,-lSize}, {}, {1.0f,0.0f}}			//右上
	};

	const unsigned short lIndices[] =
	{
		//前
		0,1,2,		//三角形1つ目
		2,1,3,		//三角形2つ目
		//後
		4,6,5,		//三角形3つ目
		6,7,5,		//三角形4つ目
		//左
		8,9,10,	//三角形1つ目
		10,9,11,	//三角形2つ目
		//右
		12,14,13,	//三角形1つ目
		13,14,15,//三角形2つ目
		//下
		16,18,17,	//三角形1つ目
		17,18,19,	//三角形2つ目
		//上
		20,21,22,	//三角形3つ目
		22,21,23	//三角形4つ目
	};


	std::array<Vertex, 24>vertices;
	std::array<USHORT, 36> indices;
	for (int i = 0; i < vertices.size(); ++i)
	{
		vertices[i] = lVertices[i];
	}
	for (int i = 0; i < indices.size(); ++i)
	{
		indices[i] = lIndices[i];
	}

	BUFFER_SIZE vertBuffSize = KazBufferHelper::GetBufferSize<BUFFER_SIZE>(sizeof(Vertex), vertices.size());
	BUFFER_SIZE indexBuffSize = KazBufferHelper::GetBufferSize<BUFFER_SIZE>(sizeof(USHORT), indices.size());

	//バッファ生成-------------------------
	vertexBufferHandle = buffers->CreateBuffer(KazBufferHelper::SetVertexBufferData(vertBuffSize));
	indexBufferHandle = buffers->CreateBuffer(KazBufferHelper::SetIndexBufferData(indexBuffSize));

	commonBufferHandle = buffers->CreateBuffer(KazBufferHelper::SetConstBufferData(sizeof(CommonData)));
	particleDataHandle = buffers->CreateBuffer(KazBufferHelper::SetRWStructuredBuffer(sizeof(ParticleData) * PARTICLE_MAX_NUM));
	outputBufferHandle = buffers->CreateBuffer(KazBufferHelper::SetRWStructuredBuffer(sizeof(OutputData) * PARTICLE_MAX_NUM));
	drawCommandHandle = buffers->CreateBuffer(KazBufferHelper::SetRWStructuredBuffer(sizeof(IndirectCommand) * DRAW_CALL));

	//更新処理用
	outputMatBufferHandle = buffers->CreateBuffer(KazBufferHelper::SetRWStructuredBuffer(sizeof(OutputMatData) * PARTICLE_MAX_NUM));
	//バッファ生成-------------------------

	//転送-------------------------
	buffers->TransData(vertexBufferHandle, vertices.data(), vertBuffSize);
	buffers->TransData(indexBufferHandle, indices.data(), indexBuffSize);

	IndirectCommand command;
	command.drawArguments.IndexCountPerInstance = static_cast<UINT>(indices.size());
	command.drawArguments.InstanceCount = PARTICLE_MAX_NUM;
	command.drawArguments.StartIndexLocation = 0;
	command.drawArguments.StartInstanceLocation = 0;
	command.drawArguments.BaseVertexLocation = 0;

	command.uav = buffers->GetGpuAddress(outputMatBufferHandle);
	buffers->TransData(drawCommandHandle, &command, sizeof(IndirectCommand) * DRAW_CALL);
	//転送-------------------------


	outputInitViewHandle = UavViewHandleMgr::Instance()->GetHandle();
	computeMemSize = DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_TEXTURE_COMPUTEBUFFER);
	DescriptorHeapMgr::Instance()->CreateBufferView(
		outputInitViewHandle,
		KazBufferHelper::SetUnorderedAccessView(sizeof(OutputData), PARTICLE_MAX_NUM),
		buffers->GetBufferData(outputBufferHandle).Get(),
		nullptr
	);

	particleViewHandle = UavViewHandleMgr::Instance()->GetHandle();
	DescriptorHeapMgr::Instance()->CreateBufferView(
		particleViewHandle,
		KazBufferHelper::SetUnorderedAccessView(sizeof(ParticleData), PARTICLE_MAX_NUM),
		buffers->GetBufferData(particleDataHandle).Get(),
		nullptr
	);

	outputMatHandle = UavViewHandleMgr::Instance()->GetHandle();
	DescriptorHeapMgr::Instance()->CreateBufferView(
		outputMatHandle,
		KazBufferHelper::SetUnorderedAccessView(sizeof(OutputMatData), PARTICLE_MAX_NUM),
		buffers->GetBufferData(outputMatBufferHandle).Get(),
		nullptr
	);

	vertexBufferView = KazBufferHelper::SetVertexBufferView(buffers->GetGpuAddress(vertexBufferHandle), vertBuffSize, sizeof(vertices[0]));
	indexBufferView = KazBufferHelper::SetIndexBufferView(buffers->GetGpuAddress(indexBufferHandle), indexBuffSize);

	lData.seed = 0;




	//パーティクルの初期化--------------------------------------------
	DescriptorHeapMgr::Instance()->SetDescriptorHeap();
	GraphicsPipeLineMgr::Instance()->SetComputePipeLineAndRootSignature(PIPELINE_COMPUTE_NAME_PORTALLINE);

	//共通用バッファのデータ送信
	{
		lData.cameraMat = CameraMgr::Instance()->GetViewMatrix();
		lData.projectionMat = CameraMgr::Instance()->GetPerspectiveMatProjection();
		lData.billboardMat = CameraMgr::Instance()->GetMatBillBoard();
		lData.increSize = sizeof(ParticleData);
		lData.gpuAddress = buffers->GetGpuAddress(outputBufferHandle);
		lData.emittPos = { 0.0f,0.0f,0.0f,30.0f };
		++lData.seed;
		buffers->TransData(commonBufferHandle, &lData, sizeof(CommonData));
		DirectX12CmdList::Instance()->cmdList->SetComputeRootConstantBufferView(2, buffers->GetGpuAddress(commonBufferHandle));
	}

	{
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(outputInitViewHandle));
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(1, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(particleViewHandle));
	}

	DirectX12CmdList::Instance()->cmdList->Dispatch(PARTICLE_MAX_NUM / 1000, 1, 1);
	//パーティクルの初期化--------------------------------------------

	finishFlag = true;
	startFlag = false;

	baseZ = 200.0f;
	initFlag = false;
}

PortalIntermediateDirection::~PortalIntermediateDirection()
{
	DescriptorHeapMgr::Instance()->Release(outputInitViewHandle);
	DescriptorHeapMgr::Instance()->Release(particleViewHandle);
	DescriptorHeapMgr::Instance()->Release(outputMatHandle);
}

void PortalIntermediateDirection::Init(bool SHOW_NEXT_STAGE_FLAG, const KazMath::Transform3D &PORTAL_POS)
{
	disappearFlag = false;
	nextPortalFlag = false;
	finishFlag = false;
	disappearTimer = 0;
	startFlag = false;
	portalRender.data.transform = PORTAL_POS;
	baseZ = PORTAL_POS.pos.z;

	showNextStageFlag = SHOW_NEXT_STAGE_FLAG;

	rate = 0.0f;
	nextRate = 0.0f;
	initFlag = true;
}

void PortalIntermediateDirection::Finalize()
{
	disappearFlag = false;
	nextPortalFlag = false;
	finishFlag = false;
	disappearTimer = 0;
	portalRender.data.transform.pos.z = baseZ;
	startFlag = false;

	rate = 0.0f;
	nextRate = 0.0f;

	backRate = 0.0f;
	nextBackRate = 0.0f;
	initFlag = false;
}

void PortalIntermediateDirection::Update(const KazMath::Vec3<float> &POS)
{
	if (!startFlag)
	{
		backRate = 0.0f;
		nextBackRate = 0.0f;
		return;
	}
	//パーティクルの更新--------------------------------------------
	DescriptorHeapMgr::Instance()->SetDescriptorHeap();
	GraphicsPipeLineMgr::Instance()->SetComputePipeLineAndRootSignature(PIPELINE_COMPUTE_NAME_PORTALLINE_MOVE);

	//共通用バッファのデータ送信
	{
		lData.cameraMat = CameraMgr::Instance()->GetViewMatrix();
		lData.projectionMat = CameraMgr::Instance()->GetPerspectiveMatProjection();
		lData.billboardMat = CameraMgr::Instance()->GetMatBillBoard();
		lData.increSize = sizeof(ParticleData);
		lData.gpuAddress = buffers->GetGpuAddress(outputBufferHandle);
		lData.emittPos = { 0.0f,0.0f,0.0f,30.0f };
		++lData.seed;
		buffers->TransData(commonBufferHandle, &lData, sizeof(CommonData));
		DirectX12CmdList::Instance()->cmdList->SetComputeRootConstantBufferView(2, buffers->GetGpuAddress(commonBufferHandle));
	}

	{
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(outputMatHandle));
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(1, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(outputInitViewHandle));
	}

	DirectX12CmdList::Instance()->cmdList->Dispatch(PARTICLE_MAX_NUM / 1000, 1, 1);
	//パーティクルの更新--------------------------------------------
	if (!finishFlag)
	{
		//今ポータルと次ポータルのZ移動
		if (nextPortalFlag)
		{
			Rate(&nextRate, 0.01f, 1.0f);
			nextBackRate = EasingMaker(InOut, Back, nextRate);
			nextPortalRender.data.transform.pos.z += -10.0f;
		}
		else
		{
			Rate(&rate, 0.01f, 1.0f);
			backRate = EasingMaker(InOut, Back, rate);
			portalRender.data.transform.pos.z = baseZ + backRate * -200.0f;
		}


		//今ポータルと次ポータルがプレイヤーより手前に来たらフラグを出す
		if (portalRender.data.transform.pos.z <= POS.z && !nextPortalFlag)
		{
			disappearFlag = true;
		}
		else if (nextPortalRender.data.transform.pos.z <= POS.z && nextPortalFlag)
		{
			nextPortalFlag = false;
			finishFlag = true;
			return;
		}


		//ポータルが消えている時間
		if (disappearFlag && showNextStageFlag)
		{
			++disappearTimer;
			nextPortalRender.data.transform.pos.z = 700.0f;
		}
		//ポータルが一定時間消えたら次ポータルを奥から出す
		if (KazMath::ConvertSecondToFlame(5) <= disappearTimer)
		{
			disappearFlag = false;
			nextPortalFlag = true;
			disappearTimer = 0;
		}
	}
	player.pos = POS;
	player.Update();
}

void PortalIntermediateDirection::Draw(Cursor *CURSOR)
{
	if (!startFlag)
	{
		return;
	}

	renderTarget->SetRenderTarget();


	if (nextPortalFlag)
	{
		nextPortalRender.Draw();
	}

	GraphicsPipeLineMgr::Instance()->SetPipeLineAndRootSignature(PIPELINE_NAME_GPUPARTICLE_PORTAL);
	DirectX12CmdList::Instance()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DirectX12CmdList::Instance()->cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);
	DirectX12CmdList::Instance()->cmdList->IASetIndexBuffer(&indexBufferView);
	RenderTargetStatus::Instance()->ChangeBarrier(
		buffers->GetBufferData(drawCommandHandle).Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT
	);

	DirectX12CmdList::Instance()->cmdList->ExecuteIndirect
	(
		commandSig.Get(),
		1,
		buffers->GetBufferData(drawCommandHandle).Get(),
		0,
		nullptr,
		0
	);

	RenderTargetStatus::Instance()->ChangeBarrier(
		buffers->GetBufferData(drawCommandHandle).Get(),
		D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);

	player.Draw();

	if (disappearFlag || nextPortalFlag)
	{
		CURSOR->cursorPos;
	}
	renderTarget->Draw();
}

void PortalIntermediateDirection::Start()
{
	startFlag = true;
}

bool PortalIntermediateDirection::IsStart()
{
	return startFlag;
}

bool PortalIntermediateDirection::IsFinish()
{
	return finishFlag && initFlag;
}

bool PortalIntermediateDirection::DrawNextPortal()
{
	return nextPortalFlag;
}