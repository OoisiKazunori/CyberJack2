#include "MeshParticleEmitter.h"
#include"../KazLibrary/DirectXCommon/DirectX12Device.h"
#include"../KazLibrary/DirectXCommon/DirectX12CmdList.h"
#include"../KazLibrary/Pipeline/GraphicsRootSignature.h"
#include"../KazLibrary/Pipeline/GraphicsPipeLineMgr.h"
#include"../KazLibrary/Helper/KazRenderHelper.h"
#include"../KazLibrary/RenderTarget/RenderTargetStatus.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../KazLibrary/Loader/ObjResourceMgr.h"
#include"../KazLibrary/Buffer/UavViewHandleMgr.h"

MeshParticleEmitter::MeshParticleEmitter(std::vector<DirectX::XMFLOAT4> VERT_NUM, float PARTICLE_SCALE)
{
	if (VERT_NUM.size() == 0)
	{
		assert(0);
	}


	buffers = std::make_unique<CreateGpuBuffer>();


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

	std::array<Vertex, 4>vertices;
	std::array<USHORT, 6> indices;
	indices = KazRenderHelper::InitIndciesForPlanePolygon();
	KazRenderHelper::InitVerticesPos(&vertices[0].pos, &vertices[1].pos, &vertices[2].pos, &vertices[3].pos, { 0.5f,0.5f });
	KazRenderHelper::InitUvPos(&vertices[0].uv, &vertices[1].uv, &vertices[2].uv, &vertices[3].uv);

	BUFFER_SIZE vertBuffSize = KazBufferHelper::GetBufferSize<BUFFER_SIZE>(vertices.size(), sizeof(Vertex));
	BUFFER_SIZE indexBuffSize = KazBufferHelper::GetBufferSize<BUFFER_SIZE>(indices.size(), sizeof(unsigned int));

	//バッファ生成-------------------------
	//初期化用--------
	//描画情報
	vertexBufferHandle = buffers->CreateBuffer(KazBufferHelper::SetVertexBufferData(vertBuffSize));
	indexBufferHandle = buffers->CreateBuffer(KazBufferHelper::SetIndexBufferData(indexBuffSize));
	verticesDataHandle = buffers->CreateBuffer(KazBufferHelper::SetRWStructuredBuffer(sizeof(DirectX::XMFLOAT4) * static_cast<UINT>(VERT_NUM.size())));

	//共通情報
	initCommonHandle = buffers->CreateBuffer(KazBufferHelper::SetConstBufferData(sizeof(InitCommonData)));

	//出力
	outputHandle = buffers->CreateBuffer(KazBufferHelper::SetOnlyReadStructuredBuffer((sizeof(OutputData) * PARTICLE_MAX_NUM)));
	//初期化用--------

	//更新用
	updateHandle = buffers->CreateBuffer(KazBufferHelper::SetOnlyReadStructuredBuffer((sizeof(UpdateData) * PARTICLE_MAX_NUM)));
	updateCommonHandle = buffers->CreateBuffer(KazBufferHelper::SetConstBufferData(sizeof(UpdateCommonData)));

	//描画用
	drawCommandHandle = buffers->CreateBuffer(KazBufferHelper::SetRWStructuredBuffer(sizeof(IndirectCommand) * DRAW_CALL));

	//バッファ生成-------------------------

	//転送-------------------------
	buffers->TransData(vertexBufferHandle, vertices.data(), vertBuffSize);
	buffers->TransData(indexBufferHandle, indices.data(), indexBuffSize);

	{
		//頂点情報
		memcpy
		(
			buffers->GetMapAddres(verticesDataHandle),
			VERT_NUM.data(),
			VERT_NUM.size() * sizeof(DirectX::XMFLOAT4)
		);

		constBufferData.worldPos = {};
		constBufferData.vertMaxNum = static_cast<UINT>(VERT_NUM.size());
		constBufferData.bias = 80;


		bias = 80;
		prevBias = 80;
		buffers->TransData(initCommonHandle, &constBufferData, sizeof(InitCommonData));
	}

	IndirectCommand command;
	command.drawArguments.IndexCountPerInstance = static_cast<UINT>(indices.size());
	command.drawArguments.InstanceCount = PARTICLE_MAX_NUM;
	command.drawArguments.StartIndexLocation = 0;
	command.drawArguments.StartInstanceLocation = 0;
	command.drawArguments.BaseVertexLocation = 0;

	command.uav = buffers->GetGpuAddress(updateHandle);
	buffers->TransData(drawCommandHandle, &command, sizeof(IndirectCommand) * DRAW_CALL);
	//転送-------------------------

	outputViewHandle = UavViewHandleMgr::Instance()->GetHandle();
	DescriptorHeapMgr::Instance()->CreateBufferView(
		outputViewHandle,
		KazBufferHelper::SetUnorderedAccessView(sizeof(OutputData), PARTICLE_MAX_NUM),
		buffers->GetBufferData(outputHandle).Get(),
		nullptr
	);

	vertDataViewHandle = UavViewHandleMgr::Instance()->GetHandle();
	DescriptorHeapMgr::Instance()->CreateBufferView(
		vertDataViewHandle,
		KazBufferHelper::SetUnorderedAccessView(sizeof(DirectX::XMFLOAT4), static_cast<UINT>(VERT_NUM.size())),
		buffers->GetBufferData(verticesDataHandle).Get(),
		nullptr
	);

	updateViewHandle = UavViewHandleMgr::Instance()->GetHandle();
	DescriptorHeapMgr::Instance()->CreateBufferView(
		updateViewHandle,
		KazBufferHelper::SetUnorderedAccessView(sizeof(UpdateData), PARTICLE_MAX_NUM),
		buffers->GetBufferData(updateHandle).Get(),
		nullptr
	);

	vertexBufferView = KazBufferHelper::SetVertexBufferView(buffers->GetGpuAddress(vertexBufferHandle), vertBuffSize, sizeof(vertices[0]));
	indexBufferView = KazBufferHelper::SetIndexBufferView(buffers->GetGpuAddress(indexBufferHandle), indexBuffSize);



	//初期化処理--------------------------------------------
	DescriptorHeapMgr::Instance()->SetDescriptorHeap();
	GraphicsPipeLineMgr::Instance()->SetComputePipeLineAndRootSignature(PIPELINE_COMPUTE_NAME_MESHPARTICLE_INIT);

	//頂点
	DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(vertDataViewHandle));
	//出力
	DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(1, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(outputViewHandle));
	//共通用バッファのデータ送信
	DirectX12CmdList::Instance()->cmdList->SetComputeRootConstantBufferView(2, buffers->GetGpuAddress(initCommonHandle));

	UINT lNum = indexNum / 1024;
	if (lNum <= 0)
	{
		lNum = 1;
	}
	DirectX12CmdList::Instance()->cmdList->Dispatch(50, 1, 1);
	//初期化処理--------------------------------------------



	resetSceneFlag = false;

	scale = PARTICLE_SCALE;
	scaleRotaMat = KazMath::CaluScaleMatrix({ scale,scale,scale }) * KazMath::CaluRotaMatrix({ 0.0f,0.0f,0.0f });

	drawParticleFlag = false;
}

MeshParticleEmitter::~MeshParticleEmitter()
{
	DescriptorHeapMgr::Instance()->Release(vertDataViewHandle);
	DescriptorHeapMgr::Instance()->Release(outputViewHandle);
	DescriptorHeapMgr::Instance()->Release(updateViewHandle);
}

void MeshParticleEmitter::Init(const DirectX::XMMATRIX *MOTHER_MAT)
{
	motherMat = MOTHER_MAT;
	sceneNum = 0;
	resetSceneFlag = false;
	drawParticleFlag = true;
}

void MeshParticleEmitter::Update(float ALPHA)
{
	/*ImGui::Begin("Mesh");
	ImGui::Checkbox("DrawParticle", &drawParticleFlag);
	ImGui::SliderInt("Bias", &bias, 0, 100);
	ImGui::DragFloat("ParticleScale", &scale);
	ImGui::End();*/

	if (!drawParticleFlag)
	{
		return;
	}
	if (ALPHA <= 0.0f)
	{
		drawParticleFlag = false;
	}

	GraphicsPipeLineMgr::Instance()->SetComputePipeLineAndRootSignature(PIPELINE_COMPUTE_NAME_MESHPARTICLE_UPDATE);

	{
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(outputViewHandle));
		DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(1, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(updateViewHandle));
	}

	//共通用バッファのデータ送信
	{
		updateCommonData.scaleRotateBillboardMat = scaleRotaMat * CameraMgr::Instance()->GetMatBillBoard();
		updateCommonData.viewProjection = CameraMgr::Instance()->GetViewMatrix() * CameraMgr::Instance()->GetPerspectiveMatProjection();
		updateCommonData.motherMat = *motherMat;
		updateCommonData.alpha = ALPHA;
		buffers->TransData(updateCommonHandle, &updateCommonData, sizeof(UpdateCommonData));
		DirectX12CmdList::Instance()->cmdList->SetComputeRootConstantBufferView(2, buffers->GetGpuAddress(updateCommonHandle));
	}
	UINT lNum = indexNum / 1024;
	if (lNum <= 0)
	{
		lNum = 1;
	}
	DirectX12CmdList::Instance()->cmdList->Dispatch(80, 1, 1);
}

void MeshParticleEmitter::Draw()
{
	if (!drawParticleFlag)
	{
		return;
	}
	GraphicsPipeLineMgr::Instance()->SetPipeLineAndRootSignature(PIPELINE_NAME_GPUPARTICLE);
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


}
