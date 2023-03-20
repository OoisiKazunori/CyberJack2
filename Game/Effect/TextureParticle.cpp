#include "TextureParticle.h"
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

TextureParticle::TextureParticle(std::vector<VertexUv> VERT_NUM, const DirectX::XMMATRIX *MOTHER_MAT, RESOURCE_HANDLE HANDLE, float PARTICLE_SCALE, UINT PER_TRIANGLE_COUNT, UINT FACE_COUNT_NUM) :motherMat(MOTHER_MAT), scale(PARTICLE_SCALE)
{
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
	verticesDataHandle = buffers->CreateBuffer(KazBufferHelper::SetRWStructuredBuffer(sizeof(VertexUv) * static_cast<UINT>(VERT_NUM.size())));

	//共通情報
	initCommonHandle = buffers->CreateBuffer(KazBufferHelper::SetConstBufferData(sizeof(InitCommonData)));

	//出力
	outputHandle = buffers->CreateBuffer(KazBufferHelper::SetOnlyReadStructuredBuffer((sizeof(OutputData) * PARTICLE_MAX_NUM)));
	//初期化用--------

	//更新用
	updateHandle = buffers->CreateBuffer(KazBufferHelper::SetOnlyReadStructuredBuffer((sizeof(UpdateData) * PARTICLE_MAX_NUM)));
	updateCommonHandle = buffers->CreateBuffer(KazBufferHelper::SetConstBufferData(sizeof(UpdateCommonData)));
	updateFlashCommonHandle = buffers->CreateBuffer(KazBufferHelper::SetConstBufferData(sizeof(UpdateCommonFlashData)));

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
			VERT_NUM.size() * sizeof(VertexUv)
		);


		bias = 0;
		prevBias = 80;

		constBufferData.worldPos = {};
		constBufferData.vertDataNum.x = static_cast<UINT>(VERT_NUM.size());
		constBufferData.vertDataNum.y = bias;
		constBufferData.vertDataNum.z = PER_TRIANGLE_COUNT;
		constBufferData.vertDataNum.w = FACE_COUNT_NUM;

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
		KazBufferHelper::SetUnorderedAccessView(sizeof(VertexUv), static_cast<UINT>(VERT_NUM.size())),
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






	resetSceneFlag = false;

	scaleRotaMat = KazMath::CaluScaleMatrix({ scale,scale,scale }) * KazMath::CaluRotaMatrix({ 0.0f,0.0f,0.0f });

	drawParticleFlag = false;

	tex = HANDLE;

	//初期化処理--------------------------------------------
	DescriptorHeapMgr::Instance()->SetDescriptorHeap();
	GraphicsPipeLineMgr::Instance()->SetComputePipeLineAndRootSignature(PIPELINE_COMPUTE_NAME_TEXTUREPARTICLE_INIT);

	//頂点
	DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(vertDataViewHandle));
	//出力
	DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(1, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(outputViewHandle));
	//共通用バッファのデータ送信
	DirectX12CmdList::Instance()->cmdList->SetComputeRootConstantBufferView(2, buffers->GetGpuAddress(initCommonHandle));
	//テクスチャ
	DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(3, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(tex));
	DirectX12CmdList::Instance()->cmdList->Dispatch(PARTICLE_MAX_NUM / 1024, 1, 1);
	//初期化処理--------------------------------------------


}

void TextureParticle::Init()
{
}

void TextureParticle::Update(bool FLAG, bool ENABLE_BILLBOARD_FLAG)
{
	//ImGui::Begin("Be");
	//ImGui::DragFloat("PosX", &updateCommonData.flash.x);
	//ImGui::DragFloat("PosY", &updateCommonData.flash.y);
	//ImGui::End();


	DirectX::XMMATRIX lMatWorld = KazMath::CaluTransMatrix({ 0.0f,0.0f,0.0f }) * KazMath::CaluScaleMatrix({ scale,scale,scale }) * KazMath::CaluRotaMatrix({ 0.0f,0.0f,0.0f });
	if (FLAG)
	{
		GraphicsPipeLineMgr::Instance()->SetComputePipeLineAndRootSignature(PIPELINE_COMPUTE_NAME_TEXTUREPARTICLE_FLASH_UPDATE);
	}
	else
	{
		GraphicsPipeLineMgr::Instance()->SetComputePipeLineAndRootSignature(PIPELINE_COMPUTE_NAME_TEXTUREPARTICLE_UPDATE);
	}


	DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(0, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(outputViewHandle));
	DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(1, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(updateViewHandle));

	DirectX::XMMATRIX lBillBoard;
	if (ENABLE_BILLBOARD_FLAG)
	{
		lBillBoard = CameraMgr::Instance()->yBillBoardArray[0];
	}
	else
	{
		lBillBoard = DirectX::XMMatrixIdentity();
	}

	if (FLAG)
	{
		//共通用バッファのデータ送信
		updateFlashCommonData.scaleRotateBillboardMat = scaleRotaMat * lBillBoard;
		updateFlashCommonData.viewProjection = CameraMgr::Instance()->GetViewMatrix() * CameraMgr::Instance()->GetPerspectiveMatProjection();
		updateFlashCommonData.motherMat = *motherMat;
		buffers->TransData(updateFlashCommonHandle, &updateFlashCommonData, sizeof(UpdateCommonFlashData));
		DirectX12CmdList::Instance()->cmdList->SetComputeRootConstantBufferView(2, buffers->GetGpuAddress(updateFlashCommonHandle));
	}
	else
	{
		//共通用バッファのデータ送信
		updateCommonData.scaleRotateBillboardMat = scaleRotaMat * lBillBoard;
		updateCommonData.viewProjection = CameraMgr::Instance()->GetViewMatrix() * CameraMgr::Instance()->GetPerspectiveMatProjection();
		updateCommonData.motherMat = *motherMat;
		buffers->TransData(updateCommonHandle, &updateCommonData, sizeof(UpdateCommonData));
		DirectX12CmdList::Instance()->cmdList->SetComputeRootConstantBufferView(2, buffers->GetGpuAddress(updateCommonHandle));
	}
	DirectX12CmdList::Instance()->cmdList->Dispatch(PARTICLE_MAX_NUM / 1024, 1, 1);
}

void TextureParticle::Draw()
{
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
