#include "GpuParticleStage.h"
#include"../KazLibrary/DirectXCommon/DirectX12Device.h"
#include"../KazLibrary/DirectXCommon/DirectX12CmdList.h"
#include"../KazLibrary/Pipeline/GraphicsRootSignature.h"
#include"../KazLibrary/Pipeline/GraphicsPipeLineMgr.h"
#include"../KazLibrary/Helper/KazRenderHelper.h"
#include"../KazLibrary/RenderTarget/RenderTargetStatus.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"


GpuParticleStage::GpuParticleStage()
{
	buffers = std::make_unique<CreateGpuBuffer>();


	texHandle = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TestPath + "Circle.png");


	//コマンドシグネチャ---------------------------
	std::array<D3D12_INDIRECT_ARGUMENT_DESC, 2> args{};
	args[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_UNORDERED_ACCESS_VIEW;
	args[0].UnorderedAccessView.RootParameterIndex = 0;
	//args[1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_SHADER_RESOURCE_VIEW;
	//args[1].ShaderResourceView.RootParameterIndex = 0;
	args[1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

	D3D12_COMMAND_SIGNATURE_DESC desc{};
	desc.pArgumentDescs = args.data();
	desc.NumArgumentDescs = static_cast<UINT>(args.size());
	desc.ByteStride = sizeof(IndirectCommand);

	HRESULT lR=
	DirectX12Device::Instance()->dev->CreateCommandSignature(&desc, GraphicsRootSignature::Instance()->GetRootSignature(ROOTSIGNATURE_DATA_DRAW_UAV).Get(), IID_PPV_ARGS(&commandSig));
	//コマンドシグネチャ---------------------------
	if (lR != S_OK)
	{
		assert(0);
	}

	//static const int VERT_MAX_NUM = 200;
	std::array<Vertex, 4>vertices;
	//for (int i = 0; i < vertices.size(); i++)
	//{
	//	vertices[i].pos.x = (1.0f * sin((KazMath::PI_2F / static_cast<float>(vertices.size())) * i));
	//	vertices[i].pos.y = -(1.0f * cos((KazMath::PI_2F / static_cast<float>(vertices.size())) * i));
	//	vertices[i].pos.z = 0.0f;
	//}

	std::array<USHORT, 6> indices;
	//for (int i = 0; i < VERT_MAX_NUM; i++)
	//{
	//	indices[i * 3] = VERT_MAX_NUM;	//中心
	//	indices[i * 3 + 1] = static_cast<USHORT>(i);			//再利用
	//	indices[i * 3 + 2] = static_cast<USHORT>(i + 1);		//新しい頂点
	//}
	//indices[indices.size() - 1] = 0;

	//頂点データ
	KazRenderHelper::InitVerticesPos(&vertices[0].pos, &vertices[1].pos, &vertices[2].pos, &vertices[3].pos, { 0.5f,0.5f });
	KazRenderHelper::InitUvPos(&vertices[0].uv, &vertices[1].uv, &vertices[2].uv, &vertices[3].uv);
	//インデックスデータ
	indices = KazRenderHelper::InitIndciesForPlanePolygon();


	BUFFER_SIZE vertBuffSize = KazBufferHelper::GetBufferSize<BUFFER_SIZE>(sizeof(Vertex), vertices.size());
	BUFFER_SIZE indexBuffSize = KazBufferHelper::GetBufferSize<BUFFER_SIZE>(sizeof(USHORT), indices.size());

	//バッファ生成-------------------------
	vertexBufferHandle = buffers->CreateBuffer(KazBufferHelper::SetVertexBufferData(vertBuffSize));
	indexBufferHandle = buffers->CreateBuffer(KazBufferHelper::SetIndexBufferData(indexBuffSize));

	commonInitBufferHandle = buffers->CreateBuffer(KazBufferHelper::SetConstBufferData(sizeof(CommonData)));
	particleDataHandle = buffers->CreateBuffer(KazBufferHelper::SetRWStructuredBuffer(sizeof(ParticleData) * PARTICLE_MAX_NUM));
	outputInitBufferHandle = buffers->CreateBuffer(KazBufferHelper::SetRWStructuredBuffer(sizeof(OutputInitData) * PARTICLE_MAX_NUM));
	drawCommandHandle = buffers->CreateBuffer(KazBufferHelper::SetRWStructuredBuffer(sizeof(IndirectCommand) * DRAW_CALL));
	counterBufferHandle = buffers->CreateBuffer(KazBufferHelper::SetRWStructuredBuffer(sizeof(UINT)));
	//バッファ生成-------------------------

	//転送-------------------------
	buffers->TransData(vertexBufferHandle, vertices.data(), vertBuffSize);
	buffers->TransData(indexBufferHandle, indices.data(), indexBuffSize);
	UINT reset = 0;
	buffers->TransData(counterBufferHandle, &reset, sizeof(UINT));

	IndirectCommand command;
	command.drawArguments.IndexCountPerInstance = static_cast<UINT>(indices.size());
	command.drawArguments.InstanceCount = PARTICLE_MAX_NUM;
	command.drawArguments.StartIndexLocation = 0;
	command.drawArguments.StartInstanceLocation = 0;
	command.drawArguments.BaseVertexLocation = 0;

	command.uav = buffers->GetGpuAddress(outputInitBufferHandle);
	buffers->TransData(drawCommandHandle, &command, sizeof(IndirectCommand) * DRAW_CALL);
	//転送-------------------------

	computeMemSize = DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_TEXTURE_COMPUTEBUFFER);
	DescriptorHeapMgr::Instance()->CreateBufferView(
		computeMemSize.startSize + 0,
		KazBufferHelper::SetUnorderedAccessView(sizeof(OutputInitData),PARTICLE_MAX_NUM),
		buffers->GetBufferData(outputInitBufferHandle).Get(),
		buffers->GetBufferData(counterBufferHandle).Get()
	);

	DescriptorHeapMgr::Instance()->CreateBufferView(
		computeMemSize.startSize + 1,
		KazBufferHelper::SetUnorderedAccessView(sizeof(ParticleData), PARTICLE_MAX_NUM),
		buffers->GetBufferData(particleDataHandle).Get(),
		nullptr
	);


	vertexBufferView = KazBufferHelper::SetVertexBufferView(buffers->GetGpuAddress(vertexBufferHandle), vertBuffSize, sizeof(vertices[0]));
	indexBufferView = KazBufferHelper::SetIndexBufferView(buffers->GetGpuAddress(indexBufferHandle), indexBuffSize);

}

GpuParticleStage::~GpuParticleStage()
{
	DescriptorHeapMgr::Instance()->Release(computeMemSize.startSize + 0);
	DescriptorHeapMgr::Instance()->Release(computeMemSize.startSize + 1);
}

void GpuParticleStage::Update()
{
	//カリング用
	GraphicsPipeLineMgr::Instance()->SetComputePipeLineAndRootSignature(PIPELINE_COMPUTE_NAME_FLOORPARTICLE);

	{
		UINT reset = 0;
		buffers->TransData(counterBufferHandle, &reset, sizeof(UINT));
	}

	//共通用バッファのデータ送信
	{
		CommonData lData;
		lData.cameraMat = CameraMgr::Instance()->GetViewMatrix();
		lData.projectionMat = CameraMgr::Instance()->GetPerspectiveMatProjection();
		lData.billboardMat = CameraMgr::Instance()->GetMatBillBoard();
		lData.increSize = sizeof(ParticleData);
		lData.gpuAddress = buffers->GetGpuAddress(outputInitBufferHandle);
		lData.emittPos = { 0.0f,0.0f,0.0f,30.0f };
		lData.seed = KazMath::Rand<int>(100, 0);
		buffers->TransData(commonInitBufferHandle, &lData, sizeof(CommonData));
		DirectX12CmdList::Instance()->cmdList->SetComputeRootConstantBufferView(2, buffers->GetGpuAddress(commonInitBufferHandle));
	}

	{
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(computeMemSize.startSize + 0));
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(1, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(computeMemSize.startSize + 1));
	}

	DirectX12CmdList::Instance()->cmdList->Dispatch(PARTICLE_MAX_NUM / 1000, 1, 1);
}

void GpuParticleStage::Draw()
{
	GraphicsPipeLineMgr::Instance()->SetPipeLineAndRootSignature(PIPELINE_NAME_GPUPARTICLE);
	DirectX12CmdList::Instance()->cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DirectX12CmdList::Instance()->cmdList->IASetVertexBuffers(0, 1, &vertexBufferView);
	DirectX12CmdList::Instance()->cmdList->IASetIndexBuffer(&indexBufferView);
	DirectX12CmdList::Instance()->cmdList->SetGraphicsRootShaderResourceView(0, TextureResourceMgr::Instance()->buffers->GetBufferData(texHandle)->GetGPUVirtualAddress());

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
		buffers->GetBufferData(counterBufferHandle).Get(),
		0
	);

	RenderTargetStatus::Instance()->ChangeBarrier(
		buffers->GetBufferData(drawCommandHandle).Get(),
		D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS
	);
}
